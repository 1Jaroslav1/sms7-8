function [s_step] = get_s_step()
    load('odp_skokowa.mat')

    k1 = 19;
    k2 = 650 + 19;

%     moment_skoku = 19;

    y = y(k1:k2);
    u = u(k1:k2);


    n= k2 - k1;

    u_step=20;
    yp=41;

    for k=1:n
        s_step(k)=(y(k)-yp)/u_step;
    end
end

