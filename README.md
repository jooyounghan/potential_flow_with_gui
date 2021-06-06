# Potential_flow_with_nana_gui
**_2D Potential Flow Analysis_** with finite element method.

Use delaunany triangulation and delaunany refinement with chew's algorthim 

(Solution file is not uploaded because of the Volume of the file.. ㅠㅠ)
***
## How To
Program is expressed with ```nana``` GUI.

![초기화면](https://user-images.githubusercontent.com/62648344/120256764-d2748a00-c2c9-11eb-8b00-65a35eeb769a.png)

When Code runs, then initial page will be opened and has 5 steps for working.

1) Domain Setting
2) Obstacle Setting
3) Intital Velocity Setting
4) Generate the Mesh with Chew's Algorithm and Analyse with FEM
5) Show Graph

***
### Setiing Domain
 
![도메인세팅](https://user-images.githubusercontent.com/62648344/120256793-e02a0f80-c2c9-11eb-81cd-1ca798f20c8d.png)
+ Set the Domain's Width and Height.
  
  With Cartesian coordinate and ```nana``` GUI doesn't support negative number(Or that I don't know how to make it support), only positive number is possible for setting
  
  For you to understand easily, suppose that you put ```width = 3000``` and ```height = 3000``` then domain set from ```(0, 0)``` to ```(0, 3000)```, ```(3000, 3000)```, ```(3000, 0)```

***
### Setiing Obstacle

![장애물설정](https://user-images.githubusercontent.com/62648344/120256777-d7d1d480-c2c9-11eb-9b2c-c3a605612591.png)

+ Set the Obstacle's Center Point and it's features(Rectangle : Width and Height / Circle : Radius).
  
  If you click **_Setiing Obstacle_**, then subpage will popped.

![장애물 설정화면](https://user-images.githubusercontent.com/62648344/120256779-daccc500-c2c9-11eb-991c-bd0ef9fbb8d5.png)

+ You can set the Obstacle at this subpage and this program supports two kinds of the obstacles

1) Rectangle
2) Circle

***
### Initial Value

![이니셜 속도 확인](https://user-images.githubusercontent.com/62648344/120256784-ddc7b580-c2c9-11eb-9607-699cb567bb5a.png)

+ Set the Initial Value(Velocity) of the Inlet.
  
  Boundary of the domain consist of 3 parts.
  1) Inlet : Located on the left side of the domain and has uniform inlet velocity
  2) Outlet : Located on the right side of the domain and has uniform Potential Value( = 0 )
  3) wall : Located on the top and bottom side of the domain and has zero quantity of derivate of Potential toward the noraml direction.

***
After Setting three things that is explained above, then click **_Generate Analysis_** for generating the mesh with Delaunay Triangulation and Refinement anf Solving the problem with Finite Element Method.

This program uses Gauss-Elimination with Pivoting.

And uses ```std::async``` and  ```std::future``` for the calculating efficiently.

Progress is described in the console.

You can also check the graphical result with clicking **_View Result_**. Matplotlib with Python as .exe file will be used for it.

***
### Result Example

![결과화면](https://user-images.githubusercontent.com/62648344/120256797-e28c6980-c2c9-11eb-83bf-4741eaeba252.png)

1) Domain : ```width = 3000```, ```height = 3000```
2) Obsatcle : circle with ```X = 1500```, ```Y = 1500```, ```R = 1000```
3) Initial Velocity : ```V = 10```

![결과화면2](https://user-images.githubusercontent.com/62648344/120256802-e5875a00-c2c9-11eb-81c6-ac6135662d05.png)

1) Domain : ```width = 3000```, ```height = 3000```
2) Obsatcle
  - circle with ```X = 0```, ```Y = 1500```, ```R = 1000```
  - circle with ```X = 3000```, ```Y = 1500```, ```R = 1000```
3) Initial Velocity : ```V = 10```

![결과화면3](https://user-images.githubusercontent.com/62648344/120256810-e7511d80-c2c9-11eb-98d6-19f99dbdebfe.png)

1) Domain : ```width = 3000```, ```height = 3000```
2) Obsatcle : rectangle with ```X = 1500```, ```Y = 1500```, ```Width = 1000```, ```Height = 1000```
3) Initial Velocity : ```V = 10```

***
### TO DO
+ This Code has problem with generating mesh when two obstacles approach nearly each other
  
  (Distortion of the Triangulation occurs)
  
![problem](https://user-images.githubusercontent.com/62648344/120910503-64113c80-c6ba-11eb-9d6c-b05468f53a84.png)

  To deal with this problem, function ```remove_after()``` has to be modified.
   * Problem Solved with modification ('21. 6. 6)

+ I'm afraid that this program only works for certain conditions.
  
  if obstacle is small or far from the domain boundary, then chew's algorithm meets the problem for refining triangles.
  
  Study how to Improve this meshing skills and Analysis with Neural Network(I think this method would be free for dealing with the mesh problem.) 
