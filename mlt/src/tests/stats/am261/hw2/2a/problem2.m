function problem2()
    noSamples = 20;
    mean  = 100;
    n     = 1000;
    x     = zeros(noSamples);
    
    for i = 1 : noSamples
        X = (speye(n,n)-random('norm',mean,1,n,n)/n)\ones(n,1);
        x(i)=X(1);
    end
    
    hist(x,100);
    fprintf('Var: %f\n', var(x));