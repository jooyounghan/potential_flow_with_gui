# Potential_flow_with_nana_gui
**_2D Potential Flow Analysis_** with finite element method.

Use delaunany triangulation and delaunany refinement with chew's algorthim 

***
## How To
Program is expressed with ```nana``` GUI. and has 5 steps for working.

When Code runs, then initial page will be opened.

![초기화면](https://user-images.githubusercontent.com/62648344/120256764-d2748a00-c2c9-11eb-8b00-65a35eeb769a.png)

+ Setiing Domain
  ++ Set the Domain's Width and Height.
  
  with Cartesian coordinate and ```nana``` GUI doesn't support negative number(Or that I don't know how to make it support), only positive number is possible for setting
  
  For you to understand easily, suppose that you put ```width = 3000``` and ```height = 3000``` then domain set from ```(0, 0)``` to ```(0, 3000)```, ```(3000, 3000)```, ```(3000, 0)```

+ Setiing Obstacle
![장애물설정](https://user-images.githubusercontent.com/62648344/120256777-d7d1d480-c2c9-11eb-9b2c-c3a605612591.png)

![장애물 설정화면](https://user-images.githubusercontent.com/62648344/120256779-daccc500-c2c9-11eb-991c-bd0ef9fbb8d5.png)

![이니셜 속도 확인](https://user-images.githubusercontent.com/62648344/120256784-ddc7b580-c2c9-11eb-9607-699cb567bb5a.png)

![도메인세팅](https://user-images.githubusercontent.com/62648344/120256793-e02a0f80-c2c9-11eb-81cd-1ca798f20c8d.png)

![결과화면](https://user-images.githubusercontent.com/62648344/120256797-e28c6980-c2c9-11eb-83bf-4741eaeba252.png)

![결과화면2](https://user-images.githubusercontent.com/62648344/120256802-e5875a00-c2c9-11eb-81c6-ac6135662d05.png)

![결과화면3](https://user-images.githubusercontent.com/62648344/120256810-e7511d80-c2c9-11eb-98d6-19f99dbdebfe.png)
