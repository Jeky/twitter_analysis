data = load('~/data/thesis/ctweets/feature-select-result-full.txt');
plot_cls_result(data);
saveas(gcf, '~/data/thesis/ctweets/feature-select-result-full.eps', 'epsc');
close(gcf);