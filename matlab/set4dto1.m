function result = set4dto1(vec)
    assert(isequal(size(vec), [4 1]));
    coeff = vec(4);
    result = [
        vec(1) / coeff;
        vec(2) / coeff;
        vec(3) / coeff;
        1.0
    ];
