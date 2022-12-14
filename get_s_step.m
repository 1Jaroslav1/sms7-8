function [s_step] = get_s_step()
    load('odp_skokowa.mat')

    k1 = 18;
    k2 = 677;

    y = y(k1: k2);
    u = u(k1:k2);

    n= k2 - k1;

    u_step=1000;

    for k=1:n
        s_step(k)=y(k)/u_step;
    end
end

