% tweet length distribution
PATH = '/Users/jeky/data/thesis/ctweets/';

FILES = {
    'spammer-tweet-len-dist-all', ...
    'non-spammer-tweet-len-dist-all'
};

NAMES = {
    'Suspended Users', ...
    'Sampled Normal Users'
};

SYMBOLS = {'x', 'o'};

for i = 1:2
    in = sprintf('%s%s.txt', PATH, char(FILES(i)));
    data = load(in);
    
    aver = sum(data(:,1) .* data(:,2)) / sum(data(:,2))
    
    loglog(data(:,1), data(:,2), char(SYMBOLS(i)), 'DisplayName', NAMES(i));

    hold all;
end

xlabel('Length');
ylabel('# Tweets');
legend('show');
saveas(gcf, sprintf('%stweet-len-dist.eps', PATH), 'psc2');