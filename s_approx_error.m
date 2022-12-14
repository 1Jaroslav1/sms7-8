function E = s_approx_error(X)
    T1 = X(1);
    T2 = X(2);
    K=X(3);
    
    n = 650;

    y(1:n) = 0;
    u(1:n) = 1;
    
    s_step = get_s_step();
    
    alpha1 = exp(-1/T1);
    alpha2 = exp(-1/T2);
    a1 = -alpha1-alpha2;
    a2 = alpha1 * alpha2;
    b1 = K*(T1*(1-alpha1)-T2*(1-alpha2))/(T1-T2);
    b2 = K*(alpha1*T2*(1-alpha2)-alpha2*T1*(1-alpha1))/(T1-T2);
    
    for k = 3:length(s_step)
        y(k) = b1*u(k - 1) + b2*u(k-2)-a1*y(k-1)-a2*y(k-2);
    end
    
    E = sum((s_step - y).^2);
end