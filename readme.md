# C++ implementation of Voronoi diagram for points
 
C++ implementation of Fortune's sweep line algorithm :
* S. Fortune, "A sweepline algorithm for Voronoi diagrams", Algorithmica, 1986

I found also very useful and detailed description of the algorithm in this book:
* M. Berg, O. Cheong, M. Kreveld and M. Overmars, "Computational geometry: Algorithms and Applications", Springer, 3rd edition, 2008

Feel free to use and test іт and don't forget to make a reference :) Also, I would be very grateful if you report about appearing found bugs.

![Image of Voronoi diagram](./Images/voronoi-1.png) 

<br/> 
<br/> 

### Main Features:
* Seems to be stable. The errors occur mainly because of the float point arithmetics (e.g. when to generator points are close together);
* No dependencies on external libraries (except visualization);
* For visualization I use external matplotlib wrapper for C++ (python is required). More info is [here](https://github.com/lava/matplotlib-cpp).
* xCode project;

## License
This project is licensed under the MIT License - see the [license.md](license.md) file for details




