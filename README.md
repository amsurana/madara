[![Build Status](https://travis-ci.org/jredmondson/madara.svg?branch=master)](https://travis-ci.org/jredmondson/madara) [![Documentation Status](https://readthedocs.org/projects/madara/badge/?version=latest)](https://madara.readthedocs.io/en/latest/?badge=latest) [![Javadocs](https://www.javadoc.io/badge/ai.madara/madara.svg)](https://www.javadoc.io/doc/ai.madara/madara)

# About MADARA

The Multi-Agent Distributed Adaptive Resource Allocation (MADARA) Project is a middleware designed to provide core knowledge sharing capabilities for multi-agent robotics and real-time systems. MADARA includes a distributed knowledge base that aggregates changes to knowledge and sends them over an extensible transport system that currently supports UDP unicast, broadcast, and multicast, 0MQ, and RTTI DDS and PrismTech DDS.

We currently support compilation on Windows, Linux, Mac, and Android architectures using a variety of compilers such as clang, g++, and Visual Studio. We also support Java and Python ports of the C++ code base for portable multi-agent programming anywhere.

***

# Installation

[Installation](https://github.com/jredmondson/madara/wiki/Installation)

The MADARA project works closely with the Group Autonomy for Mobile Systems (GAMS) project. The installation scripts we have created there will automate installations for [Mac](https://github.com/jredmondson/madara/wiki/Installation#mac) and [Linux](https://github.com/jredmondson/madara/wiki/Installation#linux) users. For Android users, you will have to install the NDK / Android Studio separately and then you can use the same [Linux scripts](https://github.com/jredmondson/madara/wiki/Installation#linux). For [Windows users](https://github.com/jredmondson/madara/wiki/Installation#windows), the guide is a bit more user-error prone. We do have build scripts that use the Visual Studio command line, but VS2017 appears to have slowed down the build process with msbuild to a crawl, and we recommend simply building the projects generated by MPC directly to get everything compiled quickly.

***

# Help

We have an extensive Wiki on this site that covers common usage patterns, performance considerations, and guides
for C++, Java and Python. Below, you'll find starting points. A more extensive list of Wiki pages can be found
on the [Wiki](https://github.com/jredmondson/madara/wiki) tab.

[Getting Started with MADARA](https://github.com/jredmondson/madara/wiki/Getting-Started) | [Youtube Guide to Multi-Agent Autonomy](https://www.youtube.com/watch?v=Cuaxt0Ow7DI&list=PL2htjCHh_RcyqGXpHY6fSt3skqxJRiBH3)

[C++ API Docs](http://madara.readthedocs.io/) | [C++ Tutorial](https://github.com/jredmondson/madara/wiki/MadaraArchitecture/)\
[Java Docs](https://www.javadoc.io/doc/ai.madara/madara) | [Java Tutorial](https://github.com/jredmondson/madara/wiki/JavaMadaraArchitecture/)\
[Python Tutorial](https://github.com/jredmondson/madara/wiki/PythonMadaraArchitecture/)
