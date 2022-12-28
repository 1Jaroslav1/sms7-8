function [Ku, Ke] = DMC_params(D, N, Nu, lambda)
    s = load("aprox_step_resp.mat").s_step_response;
    s(650:800) = s(650);
    
    M = zeros(N, Nu);
    for i = 1:N
        M(i,1)=s(i);
    end
    for i=2:Nu
        M(i:N,i)=M(1:N-i+1,1);
    end

    MP=zeros(N,D-1);
    for i=1:N
       for j=1:D-1
          MP(i,j)=s(i+j)-s(j);
       end
    end

    K = (M'*M + lambda*eye(Nu, Nu))\M';

    Ke = sum(K(1,:));
    Ku = K(1,:)*MP;
end
