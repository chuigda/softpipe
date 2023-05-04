function ctx = makecontext(width, height)
    ctx = struct;
    ctx.w = width;
    ctx.h = height;
    ctx.buffer = zeros(height, width, 3);
    ctx.depth_buffer = zeros(height, width);
    ctx.depth_buffer(:) = realmax;
