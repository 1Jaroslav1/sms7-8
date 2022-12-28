delete(instrfindall); % zamkniecie wszystkich polaczen szeregowych
clear s
clear all;
close all;
s = serialport('COM5', 115200, 'Parity', 'None');

% set(s,'BaudRate',115200);
% set(s,'StopBits',1);
% set(s,'Parity','none');
% set(s,'DataBits',8);
% set(s,'Timeout',1);
% set(s,'InputBufferSize',1000);
% set(s,'Terminator',13);

s.configureTerminator("LF");

y_zad = ones(1, 100) * 1000;

u_line = animatedline("Color","g");
y_line = animatedline("Color","r");

fopen(s); % otwarcie kanalu komunikacyjnego
Tp = 0.1; % czas z jakim probkuje regulator
y = []; % wektor wyjsc obiektu
u = []; % wektor wejsc (sterowan) obiektu
for k=1:10000
    txt = s.readline();

    eval(char(txt')); % wykonajmy to co otrzymalismy
    y=[y;Y]; % powiekszamy wektor y o element Y
    u=[u;U]; % powiekszamy wektor u o element U
    addpoints(u_line,k,u(k));
    addpoints(y_line,k,y(k));
    drawnow;

end

E = norm(y-y_zad);
disp(E);
figure; 
plot((0:(length(y)-1))*Tp,y); % wyswietlamy y w czasie
ylim([-2100, 2100])
title("Wyjście")

figure; 
plot((0:(length(u)-1))*Tp,u); % wyswietlamy u w czasie
ylim([-2100, 2100])
title("Sterowanie")
