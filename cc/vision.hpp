#ifndef VISION_HPP_
#define VISION_HPP_

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "../pack-capture-gui/capture-gui/Robot.hpp"
#include <iostream>     // std::cout
#include "Kalman_Filter.hpp"



class Vision
{
public:
  int const RHWS = 25; // Robot Half Window Size
  int const BHWS = 25; // Ball Half Window Size
  cv::Point ball_p1;
  cv::Point ball_p2;
  cv::Point robot_p1[3];
  cv::Point robot_p2[3];
  boost::thread_group threshold_threads;
  boost::thread_group threshold_threadsNew;
  cv::Point  Ballorigin;
  cv::Point robotOrigin[3];

  vector< KalmanFilter > KF_Robot;
  KalmanFilter KF_Ball;
  // Team_Main[INDEX] - Vector de cv::Point
  //   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO TIME(.x e .y acessam a posição)
  vector< cv::Point > Team_Main;
  vector<vector< cv::Point >> TeamMainNew;
  // Team_Sec[COLOR_INDEX][INDEX] - Vector de Vector de cv::Point
  //	 CADA POSIÇÃO GUARDA UM VECTOR DE cv::Point PARA CADA COR SECUNDÁRIA DO TIME
  vector<vector< cv::Point >> Team_Sec;
  vector<vector< double >> Team_Sec_area;
  vector<vector< vector <cv::Point> >> TeamSecNew;
  vector<vector< vector <double> >> TeamSecNewArea;

  // ADV_Main[INDEX] - Vector de cv::Point
  //   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO ADVERSÁRIO(.x e .y acessam a posição)
  vector< cv::Point > Adv_Main;
  // Ball - cv::Point
  //   GUARDA A POSIÇÃO DA BOLA
  cv::Point Ball;
  cv::Point KF_Ball_point;
  std::vector<cv::Point> KF_Robot_point;
  std::vector<Robot> robot_list;
  bool Ball_lost;
  bool robot_lost[3];
  int hue[6][2];
  int saturation[6][2];
  int value[6][2];
  int areaMin[6];

  int width;
  int height;

  unsigned char **threshold = NULL;

  void set_ROI(cv::Point kf_ball_point, vector <cv::Point> kf_robot_point){
    KF_Robot_point.clear();
    KF_Ball_point = kf_ball_point;
    for (int i = 0; i < kf_robot_point.size(); i++)
    {
      KF_Robot_point.push_back(kf_robot_point[i]);
    }
  }

  bool isAnyRobotLost()
  {
    std::cout << Ball_lost << ", "  << robot_lost[0] << ", " << robot_lost[1] << ", " << robot_lost[2] << std::endl;
    return (robot_lost[0] || robot_lost[1] || robot_lost[2]);
  }

  bool isBallLost(){

    return Ball_lost;
  }
  int get_robot_list_size()
  {
    return robot_list.size();
  }

  void setWindowSize(cv::Point KF, bool isRobot, int index)
  {

    cv::Point aux;

    //Determinar o x do ponto 1
    if (KF.x-RHWS<=0)
      aux.x = 0;
    else if (KF.x-RHWS>=width)
      aux.x = width;
    else
      aux.x = KF.x-RHWS;

    //Determinar o y do ponto 1
    if (KF.y-RHWS<=0)
      aux.y = 0;
    else if (KF.y-RHWS>=height)
      aux.y = height;
    else
      aux.y = KF.y-RHWS;

    // Definir o ponto 1
    if (isRobot)
      robot_p1[index] = cv::Point(aux.x, aux.y);
    else //isBall
      ball_p1 = cv::Point(aux.x, aux.y);

    //Determinar o x do ponto 2
    if (KF.x+RHWS<=0)
      aux.x = 0;
    else if (KF.x+RHWS>=width)
      aux.x = width;
    else
      aux.x = KF.x+RHWS;

    //Determinar o y do ponto 2
    if (KF.y+RHWS<=0)
      aux.y = 0;
    else if (KF.y+RHWS>=height)
      aux.y = height;
    else
      aux.y = KF.y+RHWS;

    //Definir Ponto 2
    if (isRobot)
      robot_p2[index] = cv::Point(aux.x, aux.y);
    else //isBall
      ball_p2 = cv::Point(aux.x, aux.y);
  }

  // Verifica se a janela é realmente uma janela e não uma linha
  bool checkWindowSize(cv::Point p1, cv::Point p2)
  {
    if (abs(p1.x - p2.x) > 0 && abs(p1.y - p2.y) > 0)
      return true;
    else
      return false;
  }

  Robot get_robot_from_list(int index)
  {
    Robot r;
    if (index < robot_list.size() && index >= 0)
    return robot_list[index];
    else
    {
      std::cout << "COULD NOT GET ROBOT "<<index<<" FROM LIST (VISION). QUER ME FUDER??!" << std::endl;
      return r;
    }
  }

  cv::Point get_ball_position()
  {
    return Ball;
  }

  void setHSV(int H[6][2], int S[6][2], int V[6][2], int Amin[6])
  {
    for (int i = 0; i < 6; i++)
    {
      areaMin[i] = Amin[i];
      //  cout<<"area "<<areaMin[i]<<endl;
      for (int j = 0; j < 2; j++)
      {
        hue[i][j] = H[i][j];
        saturation[i][j] = S[i][j];
        value[i][j] = V[i][j];
      }
    }
  }

  void camshift_parallel_tracking(cv::Mat im) {
  //  cout << "1.1" << endl;
    cv::Mat dummy[10];
    cv::Mat crop[10];
    cv::Mat image_copy = im.clone();

  //  cout << "1.2" << endl;
    for (int i = 0; i < 3; i++)
    {
      //cout << "KF_Robot_point[" << i << "].x = " << KF_Robot_point[i].x << " || KF_Robot_point[" << i << "].y = " << KF_Robot_point[i].y << endl;
      setWindowSize(KF_Robot_point[i], true, i);
      if (!checkWindowSize(robot_p1[i], robot_p2[i]))
      {
        setRobotLost(i);
        return;
      }

      // robot_p1[i] = cv::Point(KF_Robot_point[i].x-RHWS<=0 ? 0 : KF_Robot_point[i].x-RHWS, KF_Robot_point[i].y-RHWS<=0 ? 0 : KF_Robot_point[i].y-RHWS);
      // robot_p2[i] = cv::Point(KF_Robot_point[i].x+RHWS>=width ? width  : KF_Robot_point[i].x+RHWS, KF_Robot_point[i].y+RHWS>=height? height: KF_Robot_point[i].y+RHWS);
      //cout << "ROBOT 2" << endl;
      robotOrigin[i] = robot_p1[i];
      //cout << "ROBOT 3" << endl;
      cv::Rect  rect(robot_p1[i],robot_p2[i]);
      //cout << "ROBOT 4" << endl;
      //cout << "Robot[" << i << "] p1(" << robot_p1[i].x << ", " << robot_p1[i].y << ")" << " - Robot[" << i << "] p2(" << robot_p2[i].x << ", " << robot_p2[i].y << ")" << endl;
      dummy[3*i] = image_copy(rect); // bug
      crop[3*i] = dummy[3*i].clone(); // bug
      dummy[1+3*i] = image_copy(rect); // bug
      crop[1+3*i] = dummy[1+3*i].clone(); // bug
      dummy[2+3*i] = image_copy(rect); // bug
      crop[2+3*i] = dummy[2+3*i].clone(); // bug
      //cout << "ROBOT 5" << endl;
    }

    setWindowSize(KF_Ball_point, false, 0);
    //cout << KF_Ball_point << endl;
    //cout << ball_p1 << ", " << ball_p2 << endl;
    if (!checkWindowSize(ball_p1, ball_p2))
    {
      Ball_lost = true;
      //cout << "1.5 return" << endl;
      return;
    }
    Ballorigin = ball_p1;
  //  cout << "1.5" << endl;



    //cout << "BALL 2" << endl;
    cv::Rect  rect(ball_p1,ball_p2);
    //cout << "Ball p1(" << ball_p1.x << ", " << ball_p1.y << ")" << " - Ball p2(" << ball_p2.x << ", " << ball_p2.y << ")" << endl;
    dummy[9] = image_copy(rect); // bug
    crop[9] = dummy[9].clone(); // bug
    //cout << "BALL 4" << endl;

  //  cout << "1.6" << endl;
    for(int i = 0; i < 3; i++) {
      //camshift_img_tracking(crop[3*i], 0, i, robot_p1[i], robot_p2[i]);
      //camshift_img_tracking(crop[1+3*i], 1, i, robot_p1[i], robot_p2[i]);
      //camshift_img_tracking(crop[2+3*i], 2, i, robot_p1[i], robot_p2[i]);
      threshold_threadsNew.add_thread(new boost::thread(&Vision::camshift_img_tracking,this, boost::ref(crop[3*i]), 0, i, robot_p1[i], robot_p2[i]));
      threshold_threadsNew.add_thread(new boost::thread(&Vision::camshift_img_tracking,this, boost::ref(crop[1+3*i]), 1, i, robot_p1[i], robot_p2[i]));
      threshold_threadsNew.add_thread(new boost::thread(&Vision::camshift_img_tracking,this, boost::ref(crop[2+3*i]), 2, i, robot_p1[i], robot_p2[i]));
    }

    camshift_img_tracking(crop[9], 4, 3, ball_p1, ball_p2);

    // Tracking Bola
    //threshold_threadsNew.add_thread(new boost::thread(&Vision::camshift_img_tracking,this, boost::ref(crop[9]), 4, 3, ball_p1, ball_p2));
    //imwrite("teste.png",crop[9]);


    //Tracking Adversário
    //threshold_threadsNew.add_thread(new boost::thread(&Vision::img_tracking,this, boost::ref(image_copy), 5));

    threshold_threadsNew.join_all();
  //  cout << "1.7" << endl;
    image_copy.release();
    for (int i = 0; i < 10; i++)
    {
      crop[i].release();
      dummy[i].release();
    }
  //  cout << "1.8" << endl;

  }

  void parallel_tracking(cv::Mat im) {
    cv::Mat image_copy = im.clone();
    cv::cvtColor(image_copy,image_copy,cv::COLOR_RGB2HSV);
    cv::medianBlur(image_copy, image_copy, 5);
    Ballorigin = cv::Point(0,0);


    for(int i =0; i<6; i++) {

      threshold_threads.add_thread(new boost::thread(&Vision::img_tracking,this, boost::ref(image_copy), (i)));
    }

    threshold_threads.join_all();
    image_copy.release();




  }

  void setRobotLost(int index)
  {
    TeamMainNew[index].clear();
    TeamSecNew[index][0].clear();
    TeamSecNew[index][1].clear();
    TeamSecNewArea[index][0].clear();
    TeamSecNewArea[index][1].clear();
    robot_lost[index] = true;
  }



  void camshift_img_tracking(cv::Mat image,int color_id, int window_id, cv::Point p1,cv::Point p2) {
    //cout << "Camshift IMG TRACKING" << endl;
  //  cout << window_id << " - 1.6.1" << endl;
    int ec,e3c,H,S,V;
    vector< vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;

    cv::cvtColor(image,image,cv::COLOR_RGB2HSV);
    cv::medianBlur(image, image, 5);


    //  cout<<width<<endl;
    cv::Mat dummy;
    cv::Mat crop;
  //  cout << window_id << " - 1.6.2" << endl;
    for(int i =0; i<image.cols; i++) { //Threshold calculations
      for(int j =0; j<image.rows; j++) {
        ec = image.cols*i + j;
        e3c = ec*3;

        H = image.data[e3c];
        S = image.data[e3c + 1];
        V = image.data[e3c + 2];

        ec = width*(i+p1.y) + (j+p1.x);
        e3c = ec*3;
        if((H>=hue[color_id][0]&&H<=hue[color_id][1])&&
        (S>=saturation[color_id][0]&&S<=saturation[color_id][1])&&
        (V>=value[color_id][0]&&V<=value[color_id][1])) {

          threshold[window_id][e3c] = 255;
          threshold[window_id][e3c+1] = 255;
          threshold[window_id][e3c+2] = 255;
        } else {
          threshold[window_id][e3c] = 0;
          threshold[window_id][e3c+1] = 0;
          threshold[window_id][e3c+2] = 0;
        }
      }
    }
  //  cout << window_id << " - 1.6.3" << endl;
    cv::Rect  rect(p1,p2);


    cv::Mat temp(height,width,CV_8UC3,threshold[window_id]);
    dummy = temp(rect);
    crop = dummy.clone();
    //cv::imwrite("image.png",image);
    cv::cvtColor(crop,crop,cv::COLOR_RGB2GRAY);

    cv::findContours(crop,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);
  //  cout << window_id << " - 1.6.5" << endl;
    switch(color_id) {

      case 0:// TEAM MAIN COLOR

      if (hierarchy.size() > 0) {
        TeamMainNew[window_id].clear();
        int index = 0;
        while(index >= 0) {

          cv::Moments moment = moments((cv::Mat)contours[index]);
          double area = contourArea(contours[index]);
          //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
          if(area >= areaMin[color_id]/100) {
            TeamMainNew[window_id].push_back(cv::Point(moment.m10/area,moment.m01/area));

          }
          else
          {
            setRobotLost(window_id);
          }
          index = hierarchy[index][0];
        }
      }

      break;


      case 1:// TEAM FIRST SECUNDARY COLOR
      if (hierarchy.size() > 0) {
        TeamSecNewArea[window_id][0].clear();
        TeamSecNew[window_id][0].clear();
        int index = 0;
        while(index >= 0) {
          cv::Moments moment = moments((cv::Mat)contours[index]);
          double area = contourArea(contours[index]);
          //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
          if(area >= areaMin[color_id]/100) {
            TeamSecNew[window_id][0].push_back(cv::Point(moment.m10/area,moment.m01/area));
            //cout << "Janela [" << window_id << "] " << "VERDE: " <<  TeamSecNew[window_id][0].size() << endl;
            TeamSecNewArea[window_id][0].push_back(area);
          }
          index = hierarchy[index][0];
        }
      }


      break;

      case 2:// TEAM SECOND SECUNDARY COLOR
      if (hierarchy.size() > 0) {
        TeamSecNewArea[window_id][1].clear();
        TeamSecNew[window_id][1].clear();
        int index = 0;
        while(index >= 0) {
          cv::Moments moment = moments((cv::Mat)contours[index]);
          double area = contourArea(contours[index]);
          //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
          if(area >= areaMin[color_id]/100) {
            TeamSecNew[window_id][1].push_back(cv::Point(moment.m10/area,moment.m01/area));
            //cout << "Janela [" << window_id << "] " << "ROSA: " <<  TeamSecNew[window_id][1].size() << endl;
            TeamSecNewArea[window_id][1].push_back(area);

          }
          index = hierarchy[index][0];
        }
      }
      break;
      case 3:// TEAM THIRD SECUNDARY COLOR
      break;

      case 4:// BALL
      if (hierarchy.size() > 0) {
        cv::Moments moment = moments((cv::Mat)contours[0]);
        double area = contourArea(contours[0]);
        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
        //cout<<"camshift"<<endl;

        if(area >= areaMin[color_id]/100) {
          Ball = cv::Point(moment.m10/area,moment.m01/area)+Ballorigin;
          Ball_lost = false;

        }
      }else{
        Ball_lost = true;
        //std::cout<<"BALL LOST"<<std::endl;

      }
      break;

      case 5:// ADVERSARY MAIN COLOR
      /*
      if (hierarchy.size() > 0) {
      Adv_Main.clear();
      int j =0;
      int index = 0;
      while(index >= 0) {
      cv::Moments moment = moments((cv::Mat)contours[index]);
      double area = contourArea(contours[index]);
      //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
      if(area >= areaMin[color_id]/100) {
      Adv_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
    }
    index = hierarchy[index][0];
  }

}*/

break;

}
//cout << window_id << " - 1.6.6" << endl;
}

void img_tracking(cv::Mat image,int color_id) {
  int ec,e3c,H,S,V;
  vector< vector<cv::Point> > contours;
  vector<cv::Vec4i> hierarchy;


  for(int i =0; i<image.rows; i++) { //Threshold calculations
    for(int j =0; j<image.cols; j++) {
      ec = image.cols*i + j;
      e3c = ec*3;

      H = image.data[e3c];
      S = image.data[e3c + 1];
      V = image.data[e3c + 2];

      if((H>=hue[color_id][0]&&H<=hue[color_id][1])&&
      (S>=saturation[color_id][0]&&S<=saturation[color_id][1])&&
      (V>=value[color_id][0]&&V<=value[color_id][1])) {

        threshold[color_id][e3c] = 255;
        threshold[color_id][e3c+1] = 255;
        threshold[color_id][e3c+2] = 255;
      } else {
        threshold[color_id][e3c] = 0;
        threshold[color_id][e3c+1] = 0;
        threshold[color_id][e3c+2] = 0;
      }
    }
  }
  cv::Mat temp(height,width,CV_8UC3,threshold[color_id]);
  cv::cvtColor(temp,temp,cv::COLOR_RGB2GRAY);
  cv::findContours(temp,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);


  switch(color_id) {

    case 0:// TEAM MAIN COLOR

    if (hierarchy.size() > 0) {
      Team_Main.clear();
      int index = 0;
      while(index >= 0) {
        cv::Moments moment = moments((cv::Mat)contours[index]);
        double area = contourArea(contours[index]);
        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
        if(area >= areaMin[color_id]/100) {
          Team_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
        }
        index = hierarchy[index][0];
      }

    }

    break;


    case 1:// TEAM FIRST SECUNDARY COLOR
    if (hierarchy.size() > 0) {
      Team_Sec_area[0].clear();
      Team_Sec[0].clear();
      int index = 0;
      while(index >= 0) {
        cv::Moments moment = moments((cv::Mat)contours[index]);
        double area = contourArea(contours[index]);
        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
        if(area >= areaMin[color_id]/100) {
          Team_Sec[0].push_back(cv::Point(moment.m10/area,moment.m01/area));
          Team_Sec_area[0].push_back(area);
        }
        index = hierarchy[index][0];
      }
    }


    break;

    case 2:// TEAM SECOND SECUNDARY COLOR
    if (hierarchy.size() > 0) {
      Team_Sec_area[1].clear();
      Team_Sec[1].clear();
      int index = 0;
      while(index >= 0) {
        cv::Moments moment = moments((cv::Mat)contours[index]);
        double area = contourArea(contours[index]);
        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
        if(area >= areaMin[color_id]/100) {
          Team_Sec[1].push_back(cv::Point(moment.m10/area,moment.m01/area));
          Team_Sec_area[1].push_back(area);

        }
        index = hierarchy[index][0];
      }
    }
    break;
    case 3:// TEAM THIRD SECUNDARY COLOR
    if (hierarchy.size() > 0) {
      Team_Sec_area[2].clear();
      Team_Sec[2].clear();
      int index = 0;
      while(index >= 0) {
        cv::Moments moment = moments((cv::Mat)contours[index]);
        double area = contourArea(contours[index]);
        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
        if(area >= areaMin[color_id]/100) {
          Team_Sec[2].push_back(cv::Point(moment.m10/area,moment.m01/area));
          Team_Sec_area[2].push_back(area);
        }
        index = hierarchy[index][0];
      }
    }
    break;

    case 5:// ADVERSARY MAIN COLOR

    if (hierarchy.size() > 0) {
      Adv_Main.clear();
      int j =0;
      int index = 0;
      while(index >= 0) {
        cv::Moments moment = moments((cv::Mat)contours[index]);
        double area = contourArea(contours[index]);
        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
        if(area >= areaMin[color_id]/100) {
          Adv_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
        }
        index = hierarchy[index][0];
      }

    }

    break;

    case 4:// BALL
    if (hierarchy.size() > 0) {
      cv::Moments moment = moments((cv::Mat)contours[0]);
      double area = contourArea(contours[0]);
      //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
      //  cout<<"img_tracking"<<endl;

      if(area >= areaMin[color_id]/100) {
        Ball = cv::Point(moment.m10/area,moment.m01/area);
        Ball_lost = false;

      }
    }else{
      Ball_lost = true;
      //std::cout<<"BALL LOST"<<std::endl;

    }
    break;

  }
}

void robot_creation_unitag() {
  //std::cout << "Old Uni Duni Tag" << std::endl;
  vector <Robot> robot;
  Robot r;
  double omax = -99999; // angulo maximo
  double omin = 99999; // angulo minimo
  //  cout<<"1"<<endl;
  cv::Point secundary;
  int index1[2] = {0,0}; // index do robo com img_tracking
  int index2[2] = {0,0}; // index do robo com img_tracking (usado para trocar o index)
  float sx,sy,px,py,tx,ty; // posilções x e y das tags primária, secundária e ternária
  int l=0; // índice do robo
  float distanceRef1 = 999999999.0;
  float distanceRef2 = 999999999.0;
  float distance = 0;
  //  cout<<"2"<<endl;
  for(int j = 0; j < Team_Main.size()&&j<=3; j++) {
    robot.push_back(r);
    distanceRef1 = 999999999.0;
    distanceRef2 = 999999999.0;


    for(int i = 0; i < 2; i++) {
      for(int k = 0; k < Team_Sec[i].size(); k++) {

        distance = calcDistance(Team_Main[j],Team_Sec[i][k]);

        if(distance <= distanceRef1) {
          distanceRef2 = distanceRef1;
          index2[0] = index1[0];
          index2[1] = index1[1];

          distanceRef1 = distance;
          index1[0] = i;
          index1[1] = k;

        } else if(distance < distanceRef2) {
          distanceRef2 = distance;
          index2[0] = i;
          index2[1] = k;
        }
      }
    }
    //  cout<<"3"<<endl;
    robot[l].position = Team_Main[j];
    //  cout<<"3.1"<<endl;
    //cout<<"index1[0]: "<<index1[0] <<" index1[1]: "<<index1[1] <<" index2[0]: "<<index2[0] <<" index2[1]: "<<index2[1] <<endl;
    //cout<<Team_Sec_area[0].size()<<Team_Sec_area[1].size()<<Team_Sec_area[2].size()<<endl;
    if(Team_Sec_area[index1[0]][index1[1]]>Team_Sec_area[index2[0]][index2[1]]) {
      robot[l].secundary = Team_Sec[index1[0]][index1[1]];
      robot[l].ternary =  Team_Sec[index2[0]][index2[1]];

      //  cout<<"Area 1 = Secundary"<<"	Area 2 = Ternary"<<endl;
    } else {
      robot[l].secundary = Team_Sec[index2[0]][index2[1]];
      robot[l].ternary = Team_Sec[index1[0]][index1[1]];
      //    cout<<"Area 2 = Secundary"<<"	Area 1 = Ternary"<<endl;
    }

    //   cout<<"3.2"<<endl;


    sx = robot[l].secundary.x;
    sy =  robot[l].secundary.y;

    px = robot[l].position.x;
    py = robot[l].position.y;

    tx = robot[l].ternary.x;
    ty =  robot[l].ternary.y;


    robot[l].orientation = atan2((sy-py)*1.3/480,(sx-px)*1.5/640);
    robot[l].orientation2 = atan2((ty-py)*1.3/480,(tx-px)*1.5/640);
    l==3? : l++;

  }
  //  cout<<"4"<<endl;
  double o = 0;

  for(int i=0; i<l; i++) {
    o = atan2(sin(robot[i].orientation2-robot[i].orientation+3.1415),cos(robot[i].orientation2-robot[i].orientation+3.1415));
    // cout<<o*180/PI<<" ";
    if(o>=35*PI/180) {

      robot_list[0].position = robot[i].position; // colocar em um vetor
      robot_list[0].secundary = robot[i].secundary; // colocar em um vetor
      robot_list[0].orientation =  robot[i].orientation;

    }else if(o<=-35*PI/180) {

      robot_list[1].position = robot[i].position; // colocar em um vetor
      robot_list[1].secundary = robot[i].secundary; // colocar em um vetor
      robot_list[1].orientation =  robot[i].orientation;

    }
    //  if((o<30*PI/180)&&(o>-30*PI/180)) {
    else{
      robot_list[2].position = robot[i].position; // colocar em um vetor
      robot_list[2].secundary = robot[i].secundary; // colocar em um vetor
      robot_list[2].orientation =  robot[i].orientation;


    }
  }

}

void camshift_robot_creation_uni_duni_tag(int window_id) {
  //std::cout << "CAMSHIFT UNI DUNI TAG" << std::endl;
  Robot robot;

  double omax = -99999; // angulo maximo
  double omin = 99999; // angulo minimo
  //cout<<"1"<<endl;
  cv::Point secundary;
  int index1[2] = {0,0}; // index do robo com img_tracking
  int index2[2] = {0,0}; // index do robo com img_tracking (usado para trocar o index)
  float sx,sy,px,py,tx,ty; // posilções x e y das tags primária, secundária e ternária
  float distanceRef1 = 999999999.0;
  float distanceRef2 = 999999999.0;
  float distance = 0;
  int main_tag;
  //cout<<"2"<<endl;
  double o = 0;

  // Verificar quantas tags amarelas tem dentro da janela
  // Verificar quais das tags amarelas está mais próxima do centro da janela
  if (TeamMainNew[window_id].size() <= 0)
  {
    setRobotLost(window_id);
    return;
  }
  else if (TeamMainNew[window_id].size() == 1)
  main_tag = 0;
  else
  main_tag = getCorrectMainTag(window_id);

  //std::cout << "5.1.2" << std::endl;
  distanceRef1 = 999999999.0;
  distanceRef2 = 999999999.0;

  if (TeamSecNew[window_id][0].size() == 0 && TeamSecNew[window_id][1].size() == 0)
  {
    setRobotLost(window_id);
    return;
  }

  //  std::cout << "5.1.3" << std::endl;
  for(int i = 0; i < 2; i++) {
    for(int k = 0; k < TeamSecNew[window_id][i].size(); k++) {

      distance = calcDistance(TeamMainNew[window_id][main_tag],TeamSecNew[window_id][i][k]);

      if(distance <= distanceRef1) {
        distanceRef2 = distanceRef1;
        index2[0] = index1[0];
        index2[1] = index1[1];

        distanceRef1 = distance;
        index1[0] = i;
        index1[1] = k;
        if(i==1)
        robot.pink=true;

      } else if(distance < distanceRef2) {
        distanceRef2 = distance;
        index2[0] = i;
        index2[1] = k;
      }
    }

  }
  //cout<<"4"<<endl;
  //std::cout << "5.1.4" << std::endl;
  //  cout<<"3"<<endl;
  robot.position = TeamMainNew[window_id][main_tag]+robotOrigin[window_id];
  //  cout<<"3.1"<<endl;
  //cout<<"index1[0]: "<<index1[0] <<" index1[1]: "<<index1[1] <<" index2[0]: "<<index2[0] <<" index2[1]: "<<index2[1] <<endl;
  //cout<<Team_Sec_area[0].size()<<Team_Sec_area[1].size()<<Team_Sec_area[2].size()<<endl;
  //  std::cout << "5.1.5" << std::endl;

  //cout<<"5"<<endl;
  if (TeamSecNewArea[window_id][index1[0]].size() > index1[1] && TeamSecNewArea[window_id][index2[0]].size() > index2[1])
  {
    if(TeamSecNewArea[window_id][index1[0]][index1[1]]>TeamSecNewArea[window_id][index2[0]][index2[1]]) {
      //  std::cout << "5.1.5.1" << std::endl;
      robot.secundary = TeamSecNew[window_id][index1[0]][index1[1]]+robotOrigin[window_id];
      robot.ternary =  TeamSecNew[window_id][index2[0]][index2[1]]+robotOrigin[window_id];
      //  std::cout << "5.1.5.2" << std::endl;
      //  cout<<"Area 1 = Secundary"<<"	Area 2 = Ternary"<<endl;
    } else {
      //  std::cout << "5.1.5.3" << std::endl;
      robot.secundary = TeamSecNew[window_id][index2[0]][index2[1]]+robotOrigin[window_id];
      robot.ternary = TeamSecNew[window_id][index1[0]][index1[1]]+robotOrigin[window_id];
      //std::cout << "5.1.5.3" << std::endl;
      //    cout<<"Area 2 = Secundary"<<"	Area 1 = Ternary"<<endl;
    }
  }
  else
  {
    // ainda não foi calibrado, não precisa achar os robôs.
    setRobotLost(window_id);
    return;
  }
  //cout<<"6"<<endl;
  //std::cout << "5.1.6" << std::endl;
  //   cout<<"3.2"<<endl;


  sx = robot.secundary.x;
  sy =  robot.secundary.y;

  px = robot.position.x;
  py = robot.position.y;

  tx = robot.ternary.x;
  ty =  robot.ternary.y;

  //std::cout << "5.1.7" << std::endl;
  robot.orientation = atan2((sy-py)*1.3/480,(sx-px)*1.5/640);
  robot.orientation2 = atan2((ty-py)*1.3/480,(tx-px)*1.5/640);

  o = atan2(sin(robot.orientation2-robot.orientation+3.1415),
  cos(robot.orientation2-robot.orientation+3.1415));
  //cout<<"Robot "<<robot_id<<"  "<<o*180/PI<<"  ";
  //std::cout << "5.1.8" << std::endl;

  //cout<<"7"<<endl;
  if(robot.pink){
    if (window_id != 2)
    {
      setRobotLost(window_id);
    }
    else
    {
      robot_list[2].position = robot.position; // colocar em um vetor
      robot_list[2].secundary = robot.secundary; // colocar em um vetor
      robot_list[2].orientation =  robot.orientation;
      robot_lost[window_id] = false;
    }
    return;

  }else  if(o>0) {
    if (window_id != 0)
    {
      setRobotLost(window_id);
    }
    else
    {
      robot_list[0].position = robot.position; // colocar em um vetor
      robot_list[0].secundary = robot.secundary; // colocar em um vetor
      robot_list[0].orientation =  robot.orientation;
      robot_lost[window_id] = false;
    }
    return;

  }else {
    if (window_id != 1)
    {
      setRobotLost(window_id);
    }
    else
    {
      robot_list[1].position = robot.position; // colocar em um vetor
      robot_list[1].secundary = robot.secundary; // colocar em um vetor
      robot_list[1].orientation =  robot.orientation;
      robot_lost[window_id] = false;
    }
    return;

  }

  setRobotLost(window_id);
  //cout<<"8"<<endl;

}

int getCorrectMainTag(int window_id)
{
  float minDistance = 99999.0;
  float distance;
  int correctTag;

  for (int i = 0; i < TeamMainNew[window_id].size(); i++)
  {
    // Compara a distancia entre a tag do frame atual com a tag do frame anterior
    distance = calcDistance(TeamMainNew[window_id][i]+robotOrigin[window_id],robot_list[window_id].position);
    if (distance <= minDistance)
    {
      minDistance = distance;
      correctTag = i;
    }
  }
  return correctTag;
}


void robot_creation_uni_duni_tag() {
  vector <Robot> robot;
  Robot r;
  //  std::cout << "5.1.1" << std::endl;
  double omax = -99999; // angulo maximo
  double omin = 99999; // angulo minimo
  //  cout<<"1"<<endl;
  cv::Point secundary;
  int index1[2] = {0,0}; // index do robo com img_tracking
  int index2[2] = {0,0}; // index do robo com img_tracking (usado para trocar o index)
  float sx,sy,px,py,tx,ty; // posilções x e y das tags primária, secundária e ternária
  int l=0; // índice do robo
  float distanceRef1 = 999999999.0;
  float distanceRef2 = 999999999.0;
  float distance = 0;
  //  cout<<"2"<<endl;
  double o = 0;
  //std::cout << "5.1.2" << std::endl;
  for(int j = 0; j < Team_Main.size()&&j<=3; j++) {
    robot.push_back(r);
    distanceRef1 = 999999999.0;
    distanceRef2 = 999999999.0;

    //  std::cout << "5.1.3" << std::endl;
    for(int i = 0; i < 2; i++) {
      for(int k = 0; k < Team_Sec[i].size(); k++) {

        distance = calcDistance(Team_Main[j],Team_Sec[i][k]);

        if(distance <= distanceRef1) {
          distanceRef2 = distanceRef1;
          index2[0] = index1[0];
          index2[1] = index1[1];

          distanceRef1 = distance;
          index1[0] = i;
          index1[1] = k;
          if(i==1)
          robot[l].pink=true;

        } else if(distance < distanceRef2) {
          distanceRef2 = distance;
          index2[0] = i;
          index2[1] = k;
        }
      }

    }
    //std::cout << "5.1.4" << std::endl;
    //  cout<<"3"<<endl;
    robot[l].position = Team_Main[j];
    //  cout<<"3.1"<<endl;
    //cout<<"index1[0]: "<<index1[0] <<" index1[1]: "<<index1[1] <<" index2[0]: "<<index2[0] <<" index2[1]: "<<index2[1] <<endl;
    //cout<<Team_Sec_area[0].size()<<Team_Sec_area[1].size()<<Team_Sec_area[2].size()<<endl;
    //  std::cout << "5.1.5" << std::endl;
    if (Team_Sec_area[index1[0]].size() > index1[1] && Team_Sec_area[index2[0]].size() > index2[1])
    {
      if(Team_Sec_area[index1[0]][index1[1]]>Team_Sec_area[index2[0]][index2[1]]) {
        //  std::cout << "5.1.5.1" << std::endl;
        robot[l].secundary = Team_Sec[index1[0]][index1[1]];
        robot[l].ternary =  Team_Sec[index2[0]][index2[1]];
        //  std::cout << "5.1.5.2" << std::endl;

        //  cout<<"Area 1 = Secundary"<<"	Area 2 = Ternary"<<endl;
      } else {
        //  std::cout << "5.1.5.3" << std::endl;
        robot[l].secundary = Team_Sec[index2[0]][index2[1]];
        robot[l].ternary = Team_Sec[index1[0]][index1[1]];
        //std::cout << "5.1.5.3" << std::endl;
        //    cout<<"Area 2 = Secundary"<<"	Area 1 = Ternary"<<endl;
      }
    }
    else
    {
      // ainda não foi calibrado, não precisa achar os robôs.
      return;
    }

    //std::cout << "5.1.6" << std::endl;
    //   cout<<"3.2"<<endl;


    sx = robot[l].secundary.x;
    sy =  robot[l].secundary.y;

    px = robot[l].position.x;
    py = robot[l].position.y;

    tx = robot[l].ternary.x;
    ty =  robot[l].ternary.y;

    //std::cout << "5.1.7" << std::endl;
    robot[l].orientation = atan2((sy-py)*1.3/480,(sx-px)*1.5/640);
    robot[l].orientation2 = atan2((ty-py)*1.3/480,(tx-px)*1.5/640);

    o = atan2(sin(robot[l].orientation2-robot[l].orientation+3.1415),
    cos(robot[l].orientation2-robot[l].orientation+3.1415));
    //cout<<"Robot "<<l<<"  "<<o*180/PI<<"  ";
    //std::cout << "5.1.8" << std::endl;
    if(robot[l].pink){
      robot_list[2].position = robot[l].position; // colocar em um vetor
      robot_list[2].secundary = robot[l].secundary; // colocar em um vetor
      robot_list[2].orientation =  robot[l].orientation;
      robot_lost[2] = false;

    }else  if(o>0) {

      robot_list[0].position = robot[l].position; // colocar em um vetor
      robot_list[0].secundary = robot[l].secundary; // colocar em um vetor
      robot_list[0].orientation =  robot[l].orientation;
      robot_lost[0] = false;

    }else {

      robot_list[1].position = robot[l].position; // colocar em um vetor
      robot_list[1].secundary = robot[l].secundary; // colocar em um vetor
      robot_list[1].orientation =  robot[l].orientation;
      robot_lost[1] = false;

    }

    l==3? : l++;

  }
  //cout<<endl;
  //std::cout << "5.1.9" << std::endl;




}
//  cout<<"4"<<endl;






void robot_creation() {
  Robot robot;
  cv::Point secundary;
  int index[2];

  //  cout<<"1"<<endl;
  float distanceRef = 999999999.0;
  float distance = 0;
  for(int j = 0; j < Team_Main.size()&&j<=3; j++) {
    for(int i = 0; i < 3; i++) {
      for(int k = 0; k < Team_Sec[i].size(); k++) {

        distance = calcDistance(Team_Main[j],Team_Sec[i][k]);
        if(distance < distanceRef) {
          distanceRef = distance;
          index[0] = i;
          index[1] = k;
        }
      }
    }
    //  cout<<"2"<<endl;
    robot.position = Team_Main[j];
    robot.secundary = Team_Sec[index[0]][index[1]];
    //    cout<<"index "<<index[0]<<" size "<<robot_list.size()<<endl;
    distanceRef = 999999999.0;
    //    cout<<"3"<<endl;
    robot_list[index[0]].position = robot.position; // colocar em um vetor
    robot_list[index[0]].secundary = robot.secundary; // colocar em um vetor
    //  cout<<"4"<<endl;

    calcOrientation(index[0]);
    //    cout<<"5"<<endl;

  }
}

void calcOrientation(int tag_id) { //Define a orientação da tag em analise;
  float sx,sy,px,py;

  sx =  robot_list[tag_id].secundary.x;
  sy =  robot_list[tag_id].secundary.y;

  px = robot_list[tag_id].position.x;
  py = robot_list[tag_id].position.y;

  robot_list[tag_id].orientation = atan2((sy-py)*1.3/480,(sx-px)*1.5/640);
  robot_list[tag_id].position.x = robot_list[tag_id].position.x;
  robot_list[tag_id].position.y = robot_list[tag_id].position.y;
}

float calcDistance(cv::Point position, cv::Point secundary) {
  return sqrt(pow(position.x-secundary.x,2) + pow(position.y-secundary.y,2));
}


Vision(int w, int h)
{
  vector< double > a;

  Ball_lost = true;
  robot_lost[0] = true;
  robot_lost[1] = true;
  robot_lost[2] = true;

  Ballorigin = cv::Point(0,0);
  Team_Sec_area.push_back(a);
  Team_Sec_area.push_back(a);
  Team_Sec_area.push_back(a);

  width = w;
  height = h;

  vector< cv::Point > p;
  vector<vector < cv::Point >> q;
  vector<vector < double >> d;
  vector< double > f;
  Robot r;
  robot_list.push_back(r);
  robot_list.push_back(r);
  robot_list.push_back(r);



  p.push_back(cv::Point(0,0));
  Team_Sec.push_back(p);
  Team_Sec.push_back(p);
  Team_Sec.push_back(p);

  Team_Main.push_back(cv::Point(0,0));
  Team_Main.push_back(cv::Point(0,0));
  Team_Main.push_back(cv::Point(0,0));

  TeamMainNew.push_back(p);
  TeamMainNew.push_back(p);
  TeamMainNew.push_back(p);

  TeamSecNew.push_back(q);
  TeamSecNew.push_back(q);
  TeamSecNew.push_back(q);

  TeamSecNew[0].push_back(p);
  TeamSecNew[1].push_back(p);
  TeamSecNew[2].push_back(p);
  TeamSecNew[0].push_back(p);
  TeamSecNew[1].push_back(p);
  TeamSecNew[2].push_back(p);

  TeamSecNewArea.push_back(d);
  TeamSecNewArea.push_back(d);
  TeamSecNewArea.push_back(d);

  TeamSecNewArea[0].push_back(f);
  TeamSecNewArea[1].push_back(f);
  TeamSecNewArea[2].push_back(f);
  TeamSecNewArea[0].push_back(f);
  TeamSecNewArea[1].push_back(f);
  TeamSecNewArea[2].push_back(f);


  KalmanFilter kf;
  KF_Robot.push_back(kf);
  KF_Robot.push_back(kf);
  KF_Robot.push_back(kf);

  threshold = (unsigned char**) malloc(10 * sizeof(unsigned char *));
  for(int i = 0; i < 6; i++)
  {
    threshold[i] =  (unsigned char*) malloc((3*(width*height + width) +3) * sizeof(unsigned char));
    if(threshold[i] == NULL)
    {
      cout<<"out of memory\n"<<endl;
    }
  }
}


~Vision()
{
  if (threshold != NULL)
  {
    for(int i = 0; i < 10; i++)
      free(threshold[i]);
    free(threshold);
  }
}



};
#endif /* VISION_HPP_ */
