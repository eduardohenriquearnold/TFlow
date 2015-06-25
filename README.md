Introduction
=============

In modern society a frequent problem is traffic management and control. With the development of new technologies such as Image vision and processing it became possible to measure traffic flow and to estimate parameters, thus making it possible to control traffic lights and infer statistics of the use of cars without the need of physical sensors, such as magnet loops, which are expensive and have complex installation process.

This project aims to use image processing techniques to estimate parameters of traffic on video streams. Parameters such as mean car velocity, flow and quantity of cars passing through the day. These information could be use to feed traffic control systems, predict congestion and for estatistical analysis of car using. The input sequence of video to be used will be obtained from a publicly available online stream which shows many key traffic points of main cities in Brazil. It is valid to note that the system will not require the installation of any new cameras, thus, the implantation cost would be very low.


Software
=========

This application was developed as a project for a Computer Vision &  Image processing at UFSC. It takes a video stream and outputs a set of data containing occupation of the ROI (Region of Interest) and flow of vehicles, thus, being possible to determine if there is congestion. A Python interface was created to display this data graphically.


Video Demonstration
====================

You can see a working demo on [this](http://youtu.be/cfmSYyEQi4s) video. It was taken from a publicly available webcam at Osmar Cunha Street, Florianopolis, Brazil.

Details
=======

More details about this application can be found at the presentation I made [here](http://eduardohenriquearnold.github.io/TFlow).

Usage
=====

* To create a configuration file

```
./TFlow.out -c confFile.xml
```

* To run the app with an existing configuration file

```
./TFlow.out confFile.xml
```

* To run the app and display results graphically

```
./TFlow.out confFile.xml | python2 plotData.py
```
