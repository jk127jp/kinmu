#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

const int staffNum = 9+1; // 社員数
const int tanmuNum =3+1; // 担務数
const int dayNum=30+1;

class Staff{
public:
  // int getID(){return id;};
  Staff();
  void setProperty(int v, int g, int *t);
  int getGroup(){return group;};
  int getTanmu(int t){return tanmu[t];};
  int getKinmu(int d){return kinmu[d];};
  void setKinmu(int d, int t);
  //int checkKinmu(int d, int t);
  int getPriority(int d, int t);
private:
  // int id;
  int valid;
  int group; // 1:左 2:中 3:右
  int tanmu[tanmuNum];
  int kinmu[dayNum+2];
};

class Kinmu{
public:
  int getNextDay(){return nextDay;};
  int getNextTanmu(){return nextTanmu;};
  int getBeforeDay(){return beforeDay;};
  int getBeforeTanmu(){return beforeTanmu;};
  int getCanWork(){return canWork;}; //働ける人数を返す
  int getStaff(){return staff;};
  void setStaff(int s){staff=s;}; //staffのidをセットする。0なら未設定
  void setNext(int d, int t);
  void setBefore(int d, int t);
  void setCanWork(int cw);
  Kinmu();
private:
  int day;
  int tanmu;
  int nextDay;
  int nextTanmu;
  int beforeDay;
  int beforeTanmu;
  int staff;
  int canWork;
};

Kinmu::Kinmu(){
  day=0;
  tanmu=0;
  nextDay=0;
  nextTanmu=0;
  beforeDay=0;
  beforeTanmu=0;
  staff=0;
  canWork=0;
}
void Kinmu::setCanWork(int cw){
  canWork=cw;
}

void Kinmu::setNext(int d, int t){
  nextDay=d;
  nextTanmu=t;
}

void Kinmu::setBefore(int d, int t){
  beforeDay=d;
  beforeTanmu=t;
}

Staff::Staff()
{
  //cout << "make!" << endl;
}

void Staff::setProperty(int v, int g, int *t){
  valid = v;
  group = g;
  int i;
  for (i=1;i<tanmuNum;i++){
    tanmu[i]=t[i];
  }
}

void Staff::setKinmu(int d, int t){
  kinmu[d]=t;
}

int Staff::getPriority(int d, int t){
  int ans;
  if (kinmu[d]+kinmu[d+1]>0){
    ans = 0;
  }else{
    ans = tanmu[t];
  }
  return ans;
}

Staff staff[staffNum];
Kinmu kinmu[dayNum][tanmuNum];

int main(){
  string str;
  ifstream fin("tanmu.csv");
  if(!fin){
    cout << "Cannot open tanmu.csv" << endl;
    return 1;
  }

  int line;
  int i;
  int bfrcomma;
  int phaze;
  int sid;
  int svalid;
  int sgroup;
  int stanmu[tanmuNum];
  int s; // staff用一時変数
  //string tmp[100];
  line=0;
  fin >> str; // 1行目はラベルなので無視
  for (line=0; line<staffNum-1; line++){
    fin >> str;
    cout << str << endl;
    phaze =0;
    bfrcomma=0;
    for (i=0; i<300; i++){
      if (str[i]==','){
	if (phaze==0){
	  sid=atoi(str.substr(0,i).c_str());
	}else if(phaze==1){
	  svalid= atoi(str.substr(bfrcomma,i-bfrcomma).c_str());
	}else if(phaze==2){
	  sgroup= atoi(str.substr(bfrcomma,i-bfrcomma).c_str());
	}else if(phaze-2<tanmuNum){
	  stanmu[phaze-2]= atoi(str.substr(bfrcomma,i-bfrcomma).c_str());
	}
	bfrcomma=i+1;
	phaze++;
      }
    }
    //cout << sgroup << endl;
    staff[sid].setProperty(svalid,sgroup,stanmu);
  }

  fin.close();

  // ここまで担務情報の読み込み


  ifstream fin2("schedule.csv");
  if(!fin2){
    cout << "Cannot open schedule.csv" << endl;
    return 1;
  }

  line=0;
  fin2 >> str; // 1行目はラベルなので無視
  for (line=0; line<staffNum-1; line++){
    fin2 >> str;
    cout << str << endl;
    phaze =0;
    bfrcomma=0;
    for (i=0; i<300; i++){
      if (str[i]==','){
	if (phaze==0){
	  sid=atoi(str.substr(0,i).c_str());
	}else if(phaze<dayNum){
	  staff[sid].setKinmu(phaze,atoi(str.substr(bfrcomma,i-bfrcomma).c_str()));
	}
	bfrcomma=i+1;
	phaze++;
      }
    }
  }

  for(s=1; s<staffNum; s++){
    staff[s].setKinmu(dayNum,0); //翌月はじめは0にしておく
  }
  fin2.close();
  
  // ここまで勤務指定の入力

  int day;
  int tanmu;
  int canwork;
  int tmp;

  for(day=1;day<dayNum;day++){
    for(tanmu=1;tanmu<tanmuNum;tanmu++){
      canwork=0;
      for(i=1;i<staffNum;i++){
	tmp=staff[i].getPriority(day,tanmu);
	if (tmp>0){
	  canwork++;
	}
      }
      kinmu[day][tanmu].setCanWork(canwork);
    }
  }
  // ここまで働ける人数を数えてkinmuに格納

  int tmp_beforeDay=0;
  int tmp_beforeTanmu=0;
  int minCanWork;
  int minDay;
  int minTanmu;

  for(day=0;day<dayNum;day++){
    for(tanmu=1;tanmu<tanmuNum;tanmu++){
      kinmu[day][tanmu].setNext(0,0);
      kinmu[day][tanmu].setBefore(dayNum+1,0);
      // 初期化
    }
  }
  
  minCanWork=0;
  while(minCanWork<staffNum+1){
    minCanWork=staffNum+1;
    for(day=1;day<dayNum;day++){
      for(tanmu=1;tanmu<tanmuNum;tanmu++){
	// cout << kinmu[day][tanmu].getBeforeDay();
	if((kinmu[day][tanmu].getCanWork() < minCanWork) && (kinmu[day][tanmu].getNextDay()==0) && (kinmu[day][tanmu].getBeforeDay()==dayNum+1)){
	  // 働ける人が最小で、優先順位にまだ登録されていない場合
	  minCanWork=kinmu[day][tanmu].getCanWork();
	  minDay=day;
	  minTanmu=tanmu;
	}
      }
    }
    
    //cout << "(" << minDay << " " << minTanmu << " " << minCanWork << ")";
    
    if(minCanWork<staffNum+1){
      cout << "(" << tmp_beforeDay << " " << tmp_beforeTanmu << " " << minDay << " " << minTanmu << " " << minCanWork << ")";
      kinmu[tmp_beforeDay][tmp_beforeTanmu].setNext(minDay,minTanmu);
      kinmu[minDay][minTanmu].setBefore(tmp_beforeDay,tmp_beforeTanmu);
      tmp_beforeDay=minDay;
      tmp_beforeTanmu=minTanmu;
      
    }else{
      kinmu[tmp_beforeDay][tmp_beforeTanmu].setNext(32,0);
    }
  }

  int day_tmp;
  int maxPriority;
  int maxID;
  int s_tmp;
  int pr;
  day=kinmu[0][0].getNextDay();
  tanmu=kinmu[0][0].getNextTanmu();
  while(day>0 && day<dayNum){
    maxPriority=0;
    s_tmp=kinmu[day][tanmu].getStaff();
    kinmu[day][tanmu].setStaff(0);
    staff[s_tmp].setKinmu(day,0);
    //s_tmp=s_tmp+1;
    for(s=s_tmp+1;s<staffNum;s++){
      pr=staff[s].getPriority(day,tanmu);
      if(pr>=maxPriority){
	maxPriority=pr+1; //OnajiPriorityNaraWakaiIDwoYusen.
	maxID=s;
      }
    }
    if(maxPriority<2){
      //NoOneCanWork
      day_tmp = kinmu[day][tanmu].getBeforeDay();
      tanmu = kinmu[day][tanmu].getBeforeTanmu();
      day = day_tmp;
    }else{
      //KinmuAssign
      //cout << "(" << day << " " << tanmu << " " << 
      kinmu[day][tanmu].setStaff(maxID);
      staff[maxID].setKinmu(day,tanmu);
      staff[maxID].setKinmu(day+1,100);//Ake
      day_tmp = kinmu[day][tanmu].getNextDay();
      tanmu = kinmu[day][tanmu].getNextTanmu();
      day = day_tmp;
    }
  }

  if(day=0){
    cout << "Error!" << endl;
  }else{
    cout << "Done." << endl;
  }
  /*
    ここまででkinmu[0][0]のnextに
    一番勤務可能者が少ない日・担務が
    割り当てられている。
    もっとも優先度が低いkinmuには、
    nextDayに32が割り当てられている。
  */

  for(day=1;day<dayNum;day++){
    cout << day << " : ";
    for(tanmu=1;tanmu<tanmuNum;tanmu++){
      cout << kinmu[day][tanmu].getStaff() << " ";
    }
    cout << endl;
  }
  
  return 0;
}
