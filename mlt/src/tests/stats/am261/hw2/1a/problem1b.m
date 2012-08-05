function problem1b()
    n     = 200;
    xAxis = -sqrt(n):0.01:sqrt(n);
    
    l = size(xAxis,2);
    f     = zeros(l);
    e     = zeros(l);
    
    for i = 1 : l
        x=xAxis(i);
        a=(gamma(n/2)/(2*pi^(n/2)*sqrt(n)^(n-1)));
        b=(2*pi^((n-1)/2)*(sqrt(n-x^2)^(n-2)));
        c=gamma((n-1)/2)*sqrt(1+x^2/(x^2-n));
        f(i) = real(a*b/c);
        e(i) = 1/(sqrt(2*pi))*exp(-x*x/2);
    end
    
    %figure('Color',[0 0 0]);
    %figure;axes('Visible','on');
    %axes('Parent',figure1,'FontSize',8);
    %title(['N = ',num2str(n)]);
    g=[ [f ] [ e ] ];
    plot(xAxis,g);
    