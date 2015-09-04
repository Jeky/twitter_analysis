% output figures of token distribution
PATH = '/Users/jeky/data/thesis/ctweets/';

FILES = {
    'all-token-frequency', ...
    'spammer-token-frequency', ...
    'non-spammer-token-frequency', ...
};

for i = 1:3
    in = sprintf('%s%s.txt', PATH, char(FILES(i)));
    out = sprintf('%s%s.eps', PATH, char(FILES(i)));
    data = load(in);
    loglog(data(:,1), data(:,2), '.');
    xlabel('Occurrence');
    ylabel('Frequency');
    saveas(gcf, out, 'psc2');
    close(gcf);
end