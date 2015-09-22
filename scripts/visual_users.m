% 2d visualaztion of user matrix
data = load('~/data/thesis/ctweets/user-matrix-tsne.txt');
N = length(data) / 2;
plot(data(1:N, 1), data(1:N, 2), '.r', 'MarkerSize', 1);
hold all;
plot(data(N+1:N*2, 1), data(N+1:N*2, 2), '.b', 'MarkerSize', 1);

lh = legend('Suspended Users', 'Normal Users');
ch = get(lh,'Children');
texth = ch(strcmp(get(ch, 'Type'), 'text'));
set(texth, {'Color'}, {'b';'r'});

saveas(gcf, '~/data/thesis/ctweets/user-matrix-tsne.eps', 'epsc');
close(gcf);