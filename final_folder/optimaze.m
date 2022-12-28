[Xs] = fmincon(@s_approx_error, [0.1, 10, 0.5], [], []);
Xs

% Zapisywanie Aproksymowanych odpowiedzi do pliku
T1 = Xs(1);
T2 = Xs(2);
K=Xs(3);

n = 650;

y(1:n) = 0;
u(1:n) = 1;

alpha1 = exp(-1/T1);
alpha2 = exp(-1/T2);
a1 = -alpha1-alpha2;
a2 = alpha1 * alpha2;
b1 = K*(T1*(1-alpha1)-T2*(1-alpha2))/(T1-T2);
b2 = K*(alpha1*T2*(1-alpha2)-alpha2*T1*(1-alpha1))/(T1-T2);
s_step_response(1:211) = 0;

for k = 3:n
    s_step_response(k) = b1*u(k - 1) + b2*u(k-2)-a1*s_step_response(k-1)-a2*s_step_response(k-2);
end

% save("step_responses_opt", "s_step_response");

figure
stairs(s_step_response)
s_step = get_s_step();
hold on;
stairs(s_step)