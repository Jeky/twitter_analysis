% retweet probability
PATH = '/Users/jeky/data/thesis/ctweets/';

FILES = {
    'spammer-retweet-rate', ...
    'non-spammer-retweet-rate'
};

NAMES = {
    'Suspended Users', ...
    'Sampled Normal Users'
};

BIN_N = 60;

for i = 1:2
    in = sprintf('%s%s.txt', PATH, char(FILES(i)));
    data = load(in);
    
    intData = floor(data * BIN_N);
    bin = [];
    for count = 0:BIN_N
        bin(length(bin) + 1) = length(find(intData == count));
    end
    
    semilogy(0:1/BIN_N:1, bin, '-', 'DisplayName', NAMES(i));
    hold all;
end

xlabel('Probability of Retweet');
ylabel('# Users');
legend('show');
saveas(gcf, sprintf('%srt-dist.eps', PATH), 'psc2');