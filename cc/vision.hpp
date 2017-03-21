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

boost::thread_group threshold_threads;

class Vision
{
public:

      vector< KalmanFilter > KF_Robot;
      KalmanFilter KF_Ball;
  // Team_Main[INDEX] - Vector de cv::Point
  //   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO TIME(.x e .y acessam a posição)
  vector< cv::Point > Team_Main;
  // Team_Sec[COLOR_INDEX][INDEX] - Vector de Vector de cv::Point
  //	 CADA POSIÇÃO GUARDA UM VECTOR DE cv::Point PARA CADA COR SECUNDÁRIA DO TIME
  vector<vector< cv::Point >> Team_Sec;
  vector<vector< double >> Team_Sec_area;

  // ADV_Main[INDEX] - Vector de cv::Point
  //   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO ADVERSÁRIO(.x e .y acessam a posição)
  vector< cv::Point > Adv_Main;
  // Ball - cv::Point
  //   GUARDA A POSIÇÃO DA BOLA
  cv::Point Ball;

  std::vector<Robot> robot_list;

  int hue[6][2];
  int saturation[6][2];
  int value[6][2];
  int areaMin[6];

  int width;
  int height;

  unsigned char **threshold = NULL;

  int get_robot_list_size()
  {
    return robot_list.size();
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

  void parallel_tracking(cv::Mat im) {
    cv::Mat image_copy = im.clone();
    cv::cvtColor(image_copy,image_copy,cv::COLOR_RGB2HSV);
    cv::medianBlur(image_copy, image_copy, 5);



    for(int i =0; i<6; i++) {

      threshold_threads.add_thread(new boost::thread(&Vision::img_tracking,this, boost::ref(image_copy), (i)));
    }

    threshold_threads.join_all();
    image_copy.release();




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


        if(area >= areaMin[color_id]/100) {
          Ball = cv::Point(moment.m10/area,moment.m01/area);


        }
      }
      break;

    }
  }

  void robot_creation_unitag() {
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

      }else  if(o>0) {

        robot_list[0].position = robot[l].position; // colocar em um vetor
        robot_list[0].secundary = robot[l].secundary; // colocar em um vetor
        robot_list[0].orientation =  robot[l].orientation;

      }else {

        robot_list[1].position = robot[l].position; // colocar em um vetor
        robot_list[1].secundary = robot[l].secundary; // colocar em um vetor
        robot_list[1].orientation =  robot[l].orientation;

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
    Team_Sec_area.push_back(a);
    Team_Sec_area.push_back(a);
    Team_Sec_area.push_back(a);

    width = w;
    height = h;

    vector< cv::Point > p;
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


    KF_Robot.push_back(KF_Ball);
    KF_Robot.push_back(KF_Ball);
    KF_Robot.push_back(KF_Ball);

    threshold = (unsigned char**) malloc(6 * sizeof(unsigned char *));
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
      for(int i = 0; i < 6; i++)
      free(threshold[i]);
      free(threshold);
    }
  }



};
#endif /* VISION_HPP_ */
