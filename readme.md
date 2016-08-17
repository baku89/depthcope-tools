# Shooting System for 'depthcope'

![](./thumb.jpg)

A set of tools for 'depthcope', which I tried 3d rotoscoping. This system uses a pair of Kinect and projector, and measures a height of each points of surface from a reference plane. Then comparing them to 'height map' of a target form, this app projects the point clouds of colours which represents whether the point is higher(red) or lower(blue) than the target height. If the difference between a target height and a measured height is less than specified torelance, it's represented as green. When all dots on the surface turn into green, the form on the plane is almost accurate comparing to target form.

[depthcope](http://baku89.com/work/depthcope)

 As result it works like 'human-powered 3d printer'.
 
## Environments
 
 - openFrameworks v0.9.4 or newer
 - the environemnt which supports [ofxMultiKinectV2](https://github.com/hanasaan/ofxMultiKinectV2). I've checked the following environments work well.
	* MacBook Pro Mid 2012 (16GB RAM, 2.7GHz i7)
	* Mac Pro Late 2013 (64GB RAM, 8 cores)

## How to build
 
This repo is a set of oF project, so you'll need to clone to one deeper directory from oF installed path. For example, `(OF_PATH)/apps/(THIS_REPO)`

## Dependencies

### Softwares

- [Dragonframe](http://www.dragonframe.com/)

### oF Addons

Please reference the each repo to setup.

- [ofxMultiKinectV2](https://github.com/hanasaan/ofxMultiKinectV2)
- [ofxTurboJpeg](https://github.com/armadillu/ofxTurboJpeg)
- [ofxDatGui](https://github.com/braitsch/ofxDatGui)
- [ofxSecondWindow](https://github.com/genekogan/ofxSecondWindow)
- [ofxCv](https://github.com/kylemcdonald/ofxCv)
- [ofxofxKinectProjectorToolkitV2](https://github.com/genekogan/ofxKinectProjectorToolkitV2)
- [ofxJSON](https://github.com/jefftimesten/ofxJSON)
- [ofxGrabCam](https://github.com/elliotwoods/ofxGrabCam)
 
 ### Others
 
 - [dragonframe-osc](https://github.com/baku89/dragonframe-osc): used to detect the events of Dragonframe to sync frame to shoot
 
 ## License

 This repository is published under a MIT License. See the included LICENSE file.
