# UC sketch

## Introduction
In data stream processing, estimating item frequencies in datastreams is an important task. The unbiased estimation toward itemfrequencies has been well studied and is acknowledged in both the-ory and practice. The sliding window model estimates frequenciesof recent items and can capture the latest characteristic of datastreams in real time. In this paper, we propose the first work thatachieves unbiased frequency estimation in sliding windows, namelyUnbiased Cleaning sketch (UC sketch). The key technique ofthe UC sketch isUnbiased Cleaningwhich can remove out-dateditems out of the sliding windows in a balanced way. Besides, wesignificantly reduce the variance of frequency estimation by twooptimization techniques,Linear ScalingandColumn Randomizing.To prove the result, we conduct rigorous mathematical analysis andreasonable experiments.

## About this repo
- `count-cm`: codes to compare Count sketch and CM sketch (not used in the paper).
- `UCS.h`: main codes of the data structures and operations of UC sketch.
- `compile.sh`: 6 arguments after `./test.out`: number of matrices, number of counters per estimator, number of estimators per matrix, window size, checkpoint (for debugging), top k number (not used in the experiment part).
- `test.cpp`: codes of taking the tests and calculating the metircs.

## Requirements
- g++

## How to make
- `./compile.sh` to take the tests. 
- You may need to change some file paths and other settings in `./test.cpp`.
- You may need to change some parameters in `./compile.sh`.
