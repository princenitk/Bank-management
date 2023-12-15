#include <iostream>
#include<windows.h>
#include<mysql.h>
#include<sstream>
#include<string>
#include<time.h>
#include <ctime>
using namespace std;

const char* hostname="localhost";
const char* username="root";
const char* password="";
const char* database="bank";
unsigned int port=3306;
const char* unixsocket=NULL;
unsigned long clientflag=0;


string intToString(long long int a)
{

    ostringstream temp;  // ostringstream is used to write a string. This is similar to sprintf() in c.
    temp << a;
    return temp.str();
}

class bankSystem{
    public:

    MYSQL* conn;
    long long int acc;
    unsigned long int amount;
    int qstate;
    static string query;
    static string date;
    string rec;
    MYSQL_RES* res;
    MYSQL_ROW row;




    void connectdatabase(){

        conn=mysql_init(0);  // initializes a object of MYSQL
        conn=mysql_real_connect(conn,hostname,username,password, database, port, unixsocket,clientflag); // This function connect to the database
        if(conn){
            cout<<"connected to database...\n";
        }
        else
            cout<<"Failed to connect to database...\n";
    }


    string get_profile(int acc_no){

            string s1 = intToString(acc_no);
            query.clear();
            rec.clear();
            query="select * from account_holder where account_no= "+s1+"";
            mysql_query(conn,query.c_str());
            res = mysql_store_result(conn);
            row=mysql_fetch_row(res);
            if(row==0){
                return "";
            }
            rec+="| ACCOUNT NUMBER : "+string(row[0])+"| NAME : "+string(row[1])+" | Email :"+string(row[2])+"| AADHAR NO : "+string(row[3])+"  \n";

            row=NULL;
            mysql_free_result(res);
            res = NULL;
            return rec;
    }


    string getTime(){
        string date;
        struct tm *newtime;
        time_t ltime;
        time(&ltime);
        newtime = localtime(&ltime);
        date=asctime(newtime);
        return date;
    }

    void deposit(){

        query.clear();
        cout<<"\tEnter Amount to deposit : ";
        cin>>amount;
        date=getTime();
        query="update account_holder set total_balance=(total_balance+ "+intToString(amount)+" ) where account_no="+ intToString(acc)+" ";
        qstate = mysql_query(conn,query.c_str());
        qstate==0 ? cout<<"Amount credited successfully: )\n" : cout<<"Sorry! Unable to deposit. \n";

        query.clear();
        query="insert into transactions(transaction_id,amount,transcation_type,date,account_no) values('"+string(generateTransId())+"','"+intToString(amount)+"','"+"Credited"+"','"+date+"','"+intToString(acc)+"')";
        qstate=mysql_query(conn,query.c_str());

        cout << "Updated Balance: " << getCustBalance() << "\n";

    }



    void withdraw(){

            cout<<"Enter Amount to withdraw : ";
            cin>>amount;
            query="select * from account_holder WHERE account_no="+intToString(acc)+"";

            mysql_query(conn,query.c_str());
            res = mysql_store_result(conn);
            row=mysql_fetch_row(res);

            if(atoi(row[4])<amount){
                cout<<"\n\tNot Sufficient Balance! \n\n Current Account Balance is: "<< row[4]<<"\n";
                return ;
            }
            date=getTime();
            query="update account_holder set total_balance=(total_balance- "+intToString(amount)+" ) where account_no="+ intToString(acc)+" ";
            qstate = mysql_query(conn,query.c_str());
            qstate==0 ? cout<<" Thanks For Banking with Us! \n" : cout<<"Sorry! Unable to Process..\n";

            query.clear();
            query="insert into transactions(transaction_id,amount,transcation_type,date,account_no) values('"+string(generateTransId())+"','"+intToString(amount)+"','"+"Debited"+"','"+string(date)+"','"+string(intToString(acc))+"')";
            qstate = mysql_query(conn,query.c_str());

            cout << "Updated Balance: " << getCustBalance() << "\n";

    }

    void createFD(){

        int period;
        cout << endl;
        cout << "You will be provided flat 6 % Interest annually  ";

        cout<<"\n\nEnter Amount to Invest : ";
        cin>>amount;

        query="select * from account_holder WHERE account_no="+intToString(acc)+"";

        mysql_query(conn,query.c_str());
        res = mysql_store_result(conn);
        row=mysql_fetch_row(res);

        if(atoi(row[4])<amount){
            cout<<"\n\tNot Sufficient Balance! \n";
            return ;
        }

        cout << endl << "Enter Time Period (in years only):  ";
        cin >> period;

        date=getTime();

        query="update account_holder set total_balance=(total_balance- "+intToString(amount)+" ) where account_no="+ intToString(acc)+" ";
        qstate = mysql_query(conn,query.c_str());

        query.clear();
        query="insert into transactions(transaction_id,amount,transcation_type,date,account_no) values('"+string(generateTransId())+"','"+string(intToString(amount))+"','"+string("Created Fixed Deposit")+"','"+string(date)+"','"+string(intToString(acc))+"')";
        qstate = mysql_query(conn,query.c_str());

        int fd_id = generateAcc_no();
        int m_amount = amount + ((amount * 6 * period)/100);

        query.clear();
        query="insert into fixed_deposit(cust_id,fd_id,p_amount,m_amount,period,interest_rate,status) values('"+string(intToString(atoi(row[5])))+"','"+string(intToString(fd_id))+"','"+string(intToString(amount))+"','"+string(intToString(m_amount))+"','"+string(intToString(period))+"','"+string("6")+"','"+string("active")+"')";
        qstate = mysql_query(conn,query.c_str());

        cout << "\n\nFixed Deposit Created Successfully. \n";
        cout << "Rs. " << amount << " has been deducted from your current account.\n";
        cout << "Updated Current Account Balance: " << getCustBalance() << "\n";
        cout << "FD No: " << fd_id;
        cout << "\n Maturity Amount: " << m_amount;
        cout << "\n Thank you for banking with us! Have a nice Day!";

    }


    void balance(){

        query="select * from account_holder WHERE account_no="+intToString(acc)+"";

        mysql_query(conn,query.c_str());
        res = mysql_store_result(conn);
        row=mysql_fetch_row(res);


        rec=" BALANCE : "+string(row[4])+" \n\n";

        row=NULL;
        mysql_free_result(res);
        res = NULL;
        cout<<"\n\n"<<rec;

    }

    void transactionHis(){

        query.clear();
        query="Select * from transactions where account_no= "+intToString(acc)+"";

        mysql_query(conn,query.c_str());
        res=mysql_store_result(conn);

        cout<<"\n\n\tTransaction Id  | Amount   |  Transaction Type  |  Date \n\n";

        while(row=mysql_fetch_row(res))
            cout<<"\t"<<" "<<row[0]<<"      "<<row[1]<<"       "<<row[2]<<"      "<<row[3]<<"\n";

        row=NULL;
        mysql_free_result(res);
        res = NULL;

    }



    string getCustBalance(){

        query="select * from account_holder WHERE account_no="+intToString(acc)+"";

        mysql_query(conn,query.c_str());
        res = mysql_store_result(conn);
        row=mysql_fetch_row(res);

        string balance=row[4];
         row=NULL;
         mysql_free_result(res);
         res = NULL;

        return balance;

    }


    int delete_account(){

        char confirm;

        cout<<"Are You Sure!...(Y/N)";
        cin>>confirm;

        if(confirm=='Y'){
            query="delete from account_holder where account_no="+intToString(acc)+" ";
            qstate= mysql_query(conn,query.c_str());
            qstate==0 ? cout<<"\n Thanks for being with us.. :)\n" : cout<<"Sorry! Unable to delete account..\n";
            return 1;
        }
        return 0;
    }

    string generateTransId(){

         srand(time(0));
            char valid[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            string trans="";

            for (int i = 0; i < 15; ++i ){
                trans += valid[rand() % 62];
            }

            return trans;
    }

    int  generateCustId(){
        long long int no=1;
        srand(time(0)); // Will get time from computer and generate no against it
        for(int i=0;i<5;i++){
            no=no*10+(rand()%9)+1;
        }
        return no;
    }



    long long int generateAcc_no();
    void create_acc();

	void logout(){}

};


string bankSystem::query="";
string bankSystem::date="";

long long int bankSystem:: generateAcc_no(){
    long long int no=1;
    srand(time(0)); // Will get time from computer and generate no against it
    for(int i=0;i<9;i++){
       no=no*10+(rand()%9)+1;
    }
    return no;
}


class homePage : public bankSystem {

    public:

    void create_acc(){

        int qstate=0;
        long long int amount;
        string aadhar;
        stringstream ss;
        long long int ac_no=generateAcc_no(); // randomly create ten digit account no

        string name, emailid, paswd;
        cin.clear();
        fflush(stdin);

        cout<<"\nEnter Your Full Name : ";
        getline(cin,name);
        cout<<endl;
        cout<<"Enter your email id ";
        getline(cin,emailid);

        cin.clear();
        fflush(stdin);
        cout<<"\nEnter Aadhar no ";
        cin>>aadhar;

        cout<<"\nEnter amount to deposit ";
        cin>> amount;

        cin.clear();
        fflush(stdin);
        long int cust_id=generateCustId();
        cout<<endl;

        ss<<"INSERT INTO account_holder(account_no, user_name,user_email, aadhar_no, total_balance,cust_id) VALUES('"+string(intToString(ac_no))+"','"+string(name)+"','"+string(emailid)+"','"+string(aadhar)+"','"+string(intToString(amount))+"','"+string(intToString(cust_id))+"')";

         string query=ss.str();
         const char *q=query.c_str();
         qstate=mysql_query(conn,q);

         if(qstate==0){
            cout<<"Welcome "<< name<<"!\n";
            cout<<"Thaks for choosing us..\n";
         }

         else
            cout<<"Failed to Insert..\n ";
        system("pause");
    }

    void welcome(){
            cout<<"\t\t*************************************************************************"<<endl;
            cout<<"\t\t-------------------------------------------------------------------------"<<endl;
            cout<<"\t\t*********************   NITK CO-OPERATIVE BANK     ***********************"<<endl;
            cout<<"\t\t-------------------------------------------------------------------------"<<endl;
            cout<<"\t\t*************************************************************************"<<endl;
            cout<<"\t\t-------------------------------------------------------------------------"<<endl;

            cout<<"Press Enter.. \n\n";

            cin.get();
        }

    void home(){
        cout<<"\n******************************************************\n"<<endl;

        cout<<"\t 1) View Account Details\n"<<endl;
        cout<<"\t 2) Deposit Amount\n"<<endl;
        cout<<"\t 3) Withdraw Amount\n"<<endl;
        cout<<"\t 4) Create Fixed Deposit (FD)\n"<<endl;
        cout<<"\t 5) Balance Inquiry\n"<<endl;
        cout<<"\t 6) Last Transaction Details\n"<<endl;
        cout<<"\t 7) Close an Account\n"<<endl;
        cout<<"\t 8) Logout\n"<<endl;
        cout<<"\t 9) Exit\n"<<endl;
    }

    string getCustName(){

        query="select * from account_holder WHERE account_no="+intToString(acc)+"";

        mysql_query(conn,query.c_str());
        res = mysql_store_result(conn);
        row=mysql_fetch_row(res);

        string nam=row[1];
         row=NULL;
         mysql_free_result(res);
         res = NULL;

        return nam;

    }




    void customer_services(){
        system("cls");
        welcome();
        cout<<"\n\n You are currently accessing account of "<<getCustName()<<"\n";
        home();

        while (true){

            int input;
            long long int ac_no;
            string s;
            cout<<"\nPlease Select...";
            cin>>input;

            switch(input){


                case 1:cout<<"\t"<<get_profile(acc);
                        break;

                case 2: deposit();
                        break;

                case 3: withdraw();
                        break;

                case 4: createFD();
                        break;


                case 5:balance();
                        break;

                case 6: transactionHis();
                        break;

                case 7: if(delete_account())
                            return ;
                        break;

                case 8: return;
                        break;

                case 9: cout<<"Thanks! for Banking with Us.. " ;
                        exit(0);

                default: cout<<"Please Enter valid key: ";


            }
        }
     }

     void clear_display(){

        system("cls");
        welcome();
     }


    bool validate_account_no(){

        cout<<"\n\n\tPlease Enter Customer Account Number: ";
        cin>>acc;

        string s=get_profile(acc);
        if(s.length()==0)
            return false;

        return true;

    }

     void home_section(){


        while(true){

            clear_display();
            cout<<"\t 1) Admin Services\n"<<endl;
            cout<<"\t 2) Create New bank Account\n"<<endl;
            cout<<"\t 3) logout\n\n"<<endl;


            int input;
            cout<<"Please select any option.. ";
            cin>>input;
            switch(input){

                case 1: if(validate_account_no()){
                        customer_services();
                        }
                        else{
                            cout<<"Invalid Account Number! \n";
                            system("pause");
                        }

                        break;

                case 2: create_acc();
                        break;

                case 3: return ;


                default:cout<<"invalid input\n";
                        system("pause");
                        break;
            }
        }

     }

};



int main()
{

    homePage obj;
    obj.connectdatabase();
    obj.welcome();
    obj.home_section();


    return 0;
}
