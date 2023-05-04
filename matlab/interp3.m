function result = interp3(vertices, p, u_interp)
    v1 = vertices(1);
    v2 = vertices(2);
    v3 = vertices(3);

    p1 = v1.v_position;
    p2 = v2.v_position;
    p3 = v3.v_position;

    [w1, w2, w3] = barycentric(p1, p2, p3, p);

    if w1 <= 0 || w2 <= 0 || w3 <= 0
        result = [];
        return;
    end

    result = u_interp(w1, w2, w3, v1, v2, v3);
    result.v_position = w1 * p1 + w2 * p2 + w3 * p3;
