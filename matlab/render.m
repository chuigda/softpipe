function ret = render(context, vertices, vs, fs, uniform, interp)
    assert(mod(length(vertices), 3) == 0);

    vertices = arrayfun(@(v) vs(v, uniform), vertices);
    for i = 1:length(vertices)
        vertices(i).v_position = set4dto1(vertices(i).v_position);
    end
    
    half_width = context.w / 2;
    half_height = context.h / 2;
    for i = 1:3:length(vertices)
        triplet = vertices(i:i+2);
        vxs = arrayfun(@(v) v.v_position(1), triplet);
        vys = arrayfun(@(v) v.v_position(2), triplet);

        xmin = min(vxs);
        ymin = min(vys);
        xmax = max(vxs);
        ymax = max(vys);

        x_start_pix = max(floor((1 + xmin) * half_width), 1);
        y_start_pix = max(floor((1 + ymin) * half_height), 1);
        x_end_pix = min(ceil((1 + xmax) * half_width), context.w);
        y_end_pix = min(ceil((1 + ymax) * half_height), context.h);

        for col = x_start_pix:x_end_pix
            for row = y_start_pix:y_end_pix
                x = col / half_width - 1.0;
                y = row / half_height - 1.0;

                frag = interp3(triplet, [x; y; 0; 1], interp);
                if isempty(frag)
                    continue
                end
                
                if frag.v_position(3) >= context.depth_buffer(row, col)
                    continue
                end
                context.depth_buffer(row, col) = frag.v_position(3);
                context.buffer(row, col, 1:3) = fs(frag, uniform);
            end
        end
    end

    ret = context;
