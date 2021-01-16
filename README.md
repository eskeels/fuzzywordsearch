# fuzzywordsearch
# Levenshtein Prefilter<br>
This code will take a word and create a feature vector from it by simply using the letter frequency. It then finds the nearest matches (using euclidean distance dropping the square root) prior to performing a levenshtein distance calculation. The original plan was to completely replace the levenshtein distance but it did not work out, the words it matched were not a good selection. I tried various feature vectors (number of syllable, length etc..) but the letter frequency was as goods as anything else and took far less computation. The code is hardwired to a levenshtein distance of 2. In terms of performance the longer the search term the more time the prefilter saves. Doing some ad hoc testing I found that a four character term it is slightly faster (maybe 50% quicker) and for a length over 10 it starts being around 11 times faster. One thing to note is that the prefilter can sometimes remove too many terms. You can increase the maximum distance from 6 to 8 to reduce the filter rate (the parameter passed to IsCloseEnough()) but there is an obvious performance penalty.<br>
# Building<br>
To build you only require cmake and g++. The steps are simply:<br>
cmake .<br>
make<br>
<br>
This will create an executable called fuzzysearch. If you run it you will be prompted for a word. Enter a word and it will print a list of suggestions using the dictionary of terms in the file words.txt.<br>
Example output:<br>
./fuzzywordsearch<br>
Enter word:<br>
baadger<br>
Len is 7 start 5 end 9<br>
WD words        Lev words<br>
adger           adger<br>
badder          badder<br>
badge           badge<br>
badged          badged<br>
badger          badger<br>
badgers         badgers<br>
badges          badges<br>
badgir          badgir<br>
...<br>
...<br>
laager          laager<br>
spadger         spadger<br>
Times: Pre-filter 0.12416 Lev 0.802815<br>
<br>
<br>
There are 2 columns. The first column contains all the words using the prefilter and the second column is from just using the levenshtein algorithm. The last line output is the time taken which will (hopefully...) show the prefilter is faster.<br> 

