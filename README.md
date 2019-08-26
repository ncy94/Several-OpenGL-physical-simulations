# Several Physical Simulation Projects
* This is the lab projects for CSC 578C in Fall 2018
* The labs use Atlas framework available [here](https://github.com/marovira/atlas)

## Overview
This repository constits of several separate subjects. They are basically some physical simulations of some computer animation topic. These subjects are:
- Simulation of three-body problem
- Practise of Bezier spline
- Simulate spring-pendulum using Lagrangian Mechanics

## Build Instructions
This repository uses cmake. To build the projects in a simplest way, `cd` to the first directory (denoted as top directory in the dir tree below) with `CMakeLists.txt`, and use the following command:
```
mkdir build
cd build
cmake ..
make
```
This will build the executables for each project in the corresponding subdirectories.

## Directory structures
```
.
├── README.md                    
└── labs                            # the top directory    
    ├── data                        # stores object files
    ├── lib                         # atlas framework
    ├── CMakeLists.txt              # top CMakeLists
    └── labs                        # source files for projects
        ├── lab02
        ├── lab03
        ├── lab04
        └── CMakeLists.xtx
```     

## Project structures
Taking the source files of lab02 for example:
```
└── labs02                       
    ├── include                     # header files
    ├── shaders                     # shader files
    └── source                      # source files
        ├── GolfBall.cpp            
        ├── GolfScene.cpp
        └── main.cpp
```
`GolfBall.cpp` is a class that describes the properties of a ball (in this context, a planet). The physical movement is defined by updating the position of the object at the specific `time` in the function `GolfBall::updateGeometry(time)`. Then the object in renderered by the function `GolfBall::renderGeometry(time)`.

`GolfScene.cpp` creates scene with GUI where all render takes place

`main.cpp` triggers the application.

## My contrubution
My work in this project includes but not limit to, mostly implementing the core function, which is, as explained above, `GolfBall::updateGeometry(time)` function. 
Other parts of the project is relied heavily on rendering and GUI, which was not the primary the purpose of this course. The codes was contributed by the TA, who is also the author the [atlas framework](https://github.com/marovira/atlas). 

## Implementation of updateGeometry
According to Newton's second law, 
<img src="https://latex.codecogs.com/svg.latex?F&space;=&space;ma" title="F = ma" />
So we can calculate the acceleration $a$ from the mass and its forces. 
There'are two methods that we can calculate the position of the object. The first methods is to use the equation for absolute time:
<img src="https://latex.codecogs.com/svg.latex?v_t&space;=&space;v_0&space;&plus;&space;a\cdot&space;t" title="v_t = v_0 + a\cdot t" />
<img src="https://latex.codecogs.com/svg.latex?p_t&space;=&space;\frac{1}{2}&space;\cdot&space;a&space;\cdot&space;t^2&space;&plus;&space;v_0&space;\cdot&space;t&space;&plus;&space;p_0" title="p_t = \frac{1}{2} \cdot a \cdot t^2 + v_0 \cdot t + p_0" />
The second method was to apply the integration for the diffrentiated time:
<img src="https://latex.codecogs.com/svg.latex?P(t)&space;=&space;P(t)&space;&plus;&space;\Delta&space;t&space;\cdot&space;v(t)" title="P(t) = P(t) + \Delta t \cdot v(t)" />
<img src="https://latex.codecogs.com/svg.latex?v(t)&space;=&space;v(t)&space;&plus;&space;\Delta&space;t&space;\cdot&space;a(t)" title="v(t) = v(t) + \Delta t \cdot a(t)" />
 This method is also called Euler Integration.

### The force between planets
According to Newton's law of Universal Gravitation, $$F = \frac{G Mm}{r^2}$$
The forced of each planet is just the sum of all vector gravities that it receives: $$F = \sum_{i=1}^{n}F_i$$

### Bezier Spline
If we're going to construct a Bezier Spline with 4 control points, let `(xc_i,yc_i,zc_i)` denotes the i-th control point, we can compute the `x` coordinate of the point on `t` of the spline,
<img src="https://latex.codecogs.com/svg.latex?x(t)&space;=&space;\begin{pmatrix}&space;xc_0\\&space;xc_1\\&space;xc_2\\&space;xc_3&space;\end{pmatrix}&space;B&space;\begin{pmatrix}&space;1&space;&&space;t&space;&&space;t^2&space;&&space;t^3&space;\end{pmatrix}" title="x(t) = \begin{pmatrix} xc_0\\ xc_1\\ xc_2\\ xc_3 \end{pmatrix} B \begin{pmatrix} 1 & t & t^2 & t^3 \end{pmatrix}" />
 where the basis matrix $B$ for Bezier is as follows:
 <img src="https://latex.codecogs.com/svg.latex?\begin{pmatrix}&space;1&space;&&space;0&space;&&space;0&space;&&space;0\\&space;-3&space;&&space;3&space;&&space;0&space;&&space;0\\&space;3&space;&&space;-6&space;&&space;3&space;&&space;0\\&space;-1&space;&&space;3&space;&&space;-3&space;&&space;1&space;\end{pmatrix}" title="\begin{pmatrix} 1 & 0 & 0 & 0\\ -3 & 3 & 0 & 0\\ 3 & -6 & 3 & 0\\ -1 & 3 & -3 & 1 \end{pmatrix}" />

The `y` and `z` coordinates can be computed in the same way.

### Spring-pendulum
Let $k$ denotes the spring constant, $M$ denotes the mass of the object attached to the wall with a spring, $m, l$ denotes the mass and length of the pendulum, which is shown as the graph below:
![](labs/data/springpendulum.png)
With Lagrangian Mechanics, we can get the following:
1. The kinetic energy is:
  <img src="https://latex.codecogs.com/svg.latex?T&space;=&space;\frac{1}{2}\dot&space;x^2&space;&plus;&space;\frac{1}{2}(\dot&space;x^2&space;&plus;&space;\dot&space;y^2)&space;=\frac{1}{2}(M&plus;m)\dot&space;x^2&plus;\frac{1}{2}ml^2\dot&space;\theta^2&space;&plus;&space;ml\cdot&space;cos\theta\cdot&space;\dot&space;x&space;\dot&space;\theta" title="T = \frac{1}{2}\dot x^2 + \frac{1}{2}(\dot x^2 + \dot y^2) =\frac{1}{2}(M+m)\dot x^2+\frac{1}{2}ml^2\dot \theta^2 + ml\cdot cos\theta\cdot \dot x \dot \theta" />
 The potential energy is: 
 <img src="https://latex.codecogs.com/svg.latex?U&space;=&space;\frac{1}{2}kx^2-mgl\cdot&space;cos\theta" title="U = \frac{1}{2}kx^2-mgl\cdot cos\theta" />
 So, 
 <img src="https://latex.codecogs.com/svg.latex?L&space;=&space;T-U&space;=\frac{1}{2}(M&plus;m)\dot&space;x^2&plus;\frac{1}{2}ml^2\dot&space;\theta^2&space;&plus;ml\cdot&space;cos\theta\cdot&space;\dot&space;x&space;\dot\theta&space;-&space;\frac{1}{2}kx^2&plus;mgl\cdot&space;cos\theta" title="L = T-U =\frac{1}{2}(M+m)\dot x^2+\frac{1}{2}ml^2\dot \theta^2 +ml\cdot cos\theta\cdot \dot x \dot\theta - \frac{1}{2}kx^2+mgl\cdot cos\theta" />
2. The canonical momentas:
<img src="https://latex.codecogs.com/svg.latex?P_x&space;=&space;\frac{\partial&space;L}{\partial&space;\dot&space;x}&space;=&space;(M&plus;m)\dot&space;x&space;&plus;&space;ml\cdot&space;cos&space;\dot\theta$$" title="P_x = \frac{\partial L}{\partial \dot x} = (M+m)\dot x + ml\cdot cos \dot\theta$$" />
<img src="https://latex.codecogs.com/svg.latex?$$P_{\theta}&space;=&space;\frac{\partial&space;L}{\partial&space;\dot&space;\theta}&space;=&space;ml\cdot&space;cos\theta&space;\dot&space;x&space;&plus;&space;ml^2\dot\theta^2$$" title="$$P_{\theta} = \frac{\partial L}{\partial \dot \theta} = ml\cdot cos\theta \dot x + ml^2\dot\theta^2$$" />
3. their derivatives of time are:
<img src="https://latex.codecogs.com/svg.latex?\frac{d}{dt}(\frac{\partial&space;L}{\partial&space;\dot&space;x})=(M&plus;m)\ddot&space;x&plus;ml\cdot&space;cos\theta&space;\cdot&space;\dot\theta^2" title="\frac{d}{dt}(\frac{\partial L}{\partial \dot x})=(M+m)\ddot x+ml\cdot cos\theta \cdot \dot\theta^2" />
<img src="https://latex.codecogs.com/svg.latex?$$\frac{d}{dt}(\frac{\partial&space;L}{\partial&space;\dot\theta})=ml\cdot&space;cos\theta\cdot\ddot&space;x&space;&plus;&space;ml^2\ddot\theta$$" title="$$\frac{d}{dt}(\frac{\partial L}{\partial \dot\theta})=ml\cdot cos\theta\cdot\ddot x + ml^2\ddot\theta$$" />
4. The canonical forces are:
<img src="https://latex.codecogs.com/svg.latex?\frac{\partial&space;L}{\partial&space;x}=-kx" title="\frac{\partial L}{\partial x}=-kx" />
<img src="https://latex.codecogs.com/svg.latex?\frac{\partial&space;L}{\partial\theta}&space;=&space;-ml\cdot\sin\theta\cdot\dot&space;x&space;\dot\theta-mgl\cdot\sin\theta" title="\frac{\partial L}{\partial\theta} = -ml\cdot\sin\theta\cdot\dot x \dot\theta-mgl\cdot\sin\theta" />
5. because we have
    <img src="https://latex.codecogs.com/svg.latex?\frac{d}{dt}(\frac{\partial&space;L}{\partial&space;\dot&space;x})=\frac{\partial&space;L}{\partial&space;x},&space;\frac{d}{dt}(\frac{\partial&space;L}{\partial&space;\dot\theta})=\frac{\partial&space;L}{\partial\theta}" title="\frac{d}{dt}(\frac{\partial L}{\partial \dot x})=\frac{\partial L}{\partial x}, \frac{d}{dt}(\frac{\partial L}{\partial \dot\theta})=\frac{\partial L}{\partial\theta}" /> 
    Thus,
<img src="https://latex.codecogs.com/svg.latex?(M&plus;m)\ddot&space;x&plus;ml\cdot\cos\theta\cdot\ddot\theta-ml\sin\cdot\dot\theta^2=-kx$$&space;$$ml\cos\theta\cdot\ddot&space;x&space;&plus;&space;ml^2\ddot\theta^2=-mgl\sin\theta" title="(M+m)\ddot x+ml\cdot\cos\theta\cdot\ddot\theta-ml\sin\cdot\dot\theta^2=-kx$$ $$ml\cos\theta\cdot\ddot x + ml^2\ddot\theta^2=-mgl\sin\theta" />
1. let <img src="https://latex.codecogs.com/svg.latex?\mu=1&plus;\frac{M}{m}" title="\mu=1+\frac{M}{m}" />,we can get the second derivatives of <img src="https://latex.codecogs.com/svg.latex?x" title="x" /> and <img src="https://latex.codecogs.com/svg.latex?\theta" title="\theta" />, which are:
<img src="https://latex.codecogs.com/svg.latex?\ddot&space;x=\frac{l\sin\theta\cdot\theta^2&plus;g\sin\theta\cos\theta-\frac{k}{m}x}{\mu-\cos^2\theta}$$&space;$$\ddot\theta=\frac{\frac{k}{m}x\cos\theta-\mu&space;g\sin\theta-l\sin\theta\cos\theta}{l(\mu-\cos^2\theta)}" title="\ddot x=\frac{l\sin\theta\cdot\theta^2+g\sin\theta\cos\theta-\frac{k}{m}x}{\mu-\cos^2\theta}$$ $$\ddot\theta=\frac{\frac{k}{m}x\cos\theta-\mu g\sin\theta-l\sin\theta\cos\theta}{l(\mu-\cos^2\theta)}" />
So these two derivatives can be used as accelarations in a polar coordinate system.

## Prensentation
I attached the GIFs which showed the animation of each projects. If you find them interesting, you're welcome to read the code on the implementation. If you have and questions and concerns, don't hesitate to send me an email!