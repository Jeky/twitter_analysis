% plot accuracy and F1 
function plot_cls_result(data)
    acc = (data(:,2) + data(:,5)) ./ (data(:,2) + data(:,3) + data(:,4) + data(:,5));
    precision = data(:,2) ./ (data(:,2) + data(:,3));
    recall = data(:,2) ./ (data(:,2) + data(:,4));
    f1 = 2 * precision .* recall ./ (precision + recall);
    
    max(f1)
    acc(f1 == max(f1))
    data(f1 == max(f1), 1)
    
    x = log10(data(:,1));
    
    plot(x, acc, '-b', 'DisplayName','Accuracy');
    hold all;
    plot(x, f1, '-r', 'DisplayName', 'F1');
    
    xlabel('log(Feature Size)');
    ylabel('Accuracy and F1');
    
    legend('show', 'Location', 'Southeast');
end