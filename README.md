# Haze_removal
Single Image Haze Removal Using Dark Channel Prior (paper implementation)

## References
The original algorithm is described in https://www.robots.ox.ac.uk/~vgg/rg/papers/hazeremoval.pdf

This implementation uses guided filtering instead of soft-matting in order to improve computationnal efficiency.

Guided filters were introduced in http://kaiminghe.com/publications/eccv10guidedfilter.pdf

## Sample Results

### Sample 1

Source image:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/canon.jpg)

Dehazed result:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/canon_dehazed.jpg)

Transmission map:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/canon_transmission.jpg)

Filtered transmission map:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/canon_filteredTransmission.jpg)

### Sample 2

Source image:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/village.png)

Dehazed result:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/village_dehazed.jpg)

Transmission map:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/village_transmission.jpg)

Filtered transmission map:

![alt text](https://github.com/chattob/Haze_removal/blob/master/sampleResults/village_filteredtransmission.jpg)
