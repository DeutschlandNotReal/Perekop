#include <PK/Physics/ModelBVH.hpp>

#include <algorithm>

using namespace pk;

namespace {
    constexpr int   leafSize{4};
    constexpr float noHit{1e30f};

    float enter(const AABB& box, vec3 o, vec3 inv, float tmax) noexcept {
        vec3 a = (box.min - o) * inv, b = (box.max - o) * inv;
        vec3 lo = glm::min(a, b), hi = glm::max(a, b);

        float tn = pk::max(lo.x, lo.y, lo.z, 0.f);
        float tf = pk::min(hi.x, hi.y, hi.z, tmax);

        return tn <= tf ? tn : -1.f;
    }
}

void ModelBVH::clear() noexcept {
    items.clear();
    nodes.clear();
}

void ModelBVH::construct(span<ModelHandle> handles) {
    clear();
    items.reserve(handles.size());

    for (const ModelHandle& model : handles)
        items.emplace(item{model.getID(), model.getWorldAABB()});

    if (items.empty()) return;

    nodes.reserve(items.size());

    auto build = [&](auto&& self, int i, int j) -> int {
        int id = nodes.size();
        nodes.emplace();

        AABB box = items[i].bounds;
        vec3 lo = box.getCentre(), hi = lo;

        for (int k{i + 1}; k < j; ++k) {
            const AABB& b = items[k].bounds;
            vec3 c = b.getCentre();

            box = AABB::combine(box, b);
            lo  = glm::min(lo, c);
            hi  = glm::max(hi, c);
        }

        nodes[id].bounds = box;

        if (j - i <= leafSize) {
            nodes[id].leaf = true;
            nodes[id].lhs  = i;
            nodes[id].rhs  = j;
            return id;
        }

        vec3 e = hi - lo;
        int axis = e.x > e.y ? (e.x > e.z ? 0 : 2) : (e.y > e.z ? 1 : 2);
        int mid  = (i + j) / 2;

        std::nth_element(
            items.begin() + i, items.begin() + mid, items.begin() + j,
            [axis](const item& a, const item& b) {
                return a.bounds.min[axis] + a.bounds.max[axis] < b.bounds.min[axis] + b.bounds.max[axis];
            }
        );

        int l = self(self, i, mid);
        int r = self(self, mid, j);

        nodes[id].lhs = l;
        nodes[id].rhs = r;

        return id;
    };

    build(build, 0, items.size());
}

span<ModelID> ModelBVH::query(AABB bounds) const noexcept {
    static thread_local vector<ModelID> out;
    out.clear();

    int stack[128], top{0};
    if (!nodes.empty()) stack[top++] = 0;

    while (top) {
        const node& n = nodes[stack[--top]];
        if (!n.bounds.intersects(bounds)) continue;

        if (n.leaf) {
            for (int k{n.lhs}; k < n.rhs; ++k)
                if (items[k].bounds.intersects(bounds)) out.emplace(items[k].id);
            continue;
        }

        stack[top++] = n.rhs;
        stack[top++] = n.lhs;
    }

    return span<ModelID>{out.begin(), out.end()};
}

ModelBVH::rayresult ModelBVH::cast(ray r) const noexcept {
    rayresult res{.dst = noHit};
    if (nodes.empty()) return res;

    struct entry { int id; float t; } stack[128];
    int top{0};

    float t0 = enter(nodes[0].bounds, r.pos, r.invdir, res.dst);
    if (t0 >= 0.f) stack[top++] = {0, t0};

    while (top) {
        auto [id, t] = stack[--top];
        if (t > res.dst) continue;

        const node& n = nodes[id];

        if (n.leaf) {
            for (int k{n.lhs}; k < n.rhs; ++k) {
                float td = enter(items[k].bounds, r.pos, r.invdir, res.dst);
                if (td < 0.f) continue;

                res.dst = td;
                res.hit = items[k].id;
            }
            continue;
        }

        float tl = enter(nodes[n.lhs].bounds, r.pos, r.invdir, res.dst);
        float tr = enter(nodes[n.rhs].bounds, r.pos, r.invdir, res.dst);

        if (tl >= 0.f && tr >= 0.f) {
            if (tl < tr) { stack[top++] = {n.rhs, tr}; stack[top++] = {n.lhs, tl}; }
            else         { stack[top++] = {n.lhs, tl}; stack[top++] = {n.rhs, tr}; }
        }
        else if (tl >= 0.f) stack[top++] = {n.lhs, tl};
        else if (tr >= 0.f) stack[top++] = {n.rhs, tr};
    }

    if (res.dst < noHit) res.pos = r.pos + r.dir * res.dst;
    return res;
}