#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <mysql/jdbc.h>
#include <mutex>
#include <thread>
#include <chrono>
std::mutex mtx;
bool ok;
void show_table(const std::string & table,sql::Connection *con){

    std::string sql_statement="select * from "+table ;
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt=con->createStatement();
    res=stmt->executeQuery(sql_statement);
    mtx.lock();
    while(res->next()){
        std::cout<<"id: "<<res->getInt(1)<<" ";
        std::cout<<"| name of item : "<<res->getString(2);
        std::cout<<std::endl;
    }
    mtx.unlock();
    delete res;
    delete stmt;
    
}
void command_done(const std::string & table , sql::Connection *con){

    std::string sql_statement="delete from "+table+" where id=? ";
    mtx.lock();
    std::cout<<"enter what order did you finsih , based on id ? (0 means exiting the program) ";
    mtx.unlock();
    int x;
    std::cin>>x;
    sql::PreparedStatement *prep_stmt;
    prep_stmt=con->prepareStatement(sql_statement);
    while(x!=0){
        prep_stmt->setInt(1,x);
        prep_stmt->execute();
        std::cin>>x;
    }
    if(x==0){
        std::cout<<"program is finished";
        ok=false;
    }
    
    delete prep_stmt;
}
void showing(const std::string &table,sql::Connection *con){
    while(ok){
        show_table(table,con);
        std::cout<<"updating"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
     
}
int main()
{
    const std::string url    = "tcp://127.0.0.1:3306";
    const std::string user   = "mysql";
    const std::string pass   = "mysql1234";
    const std::string schema = "mydb";
    const std::string cook_tb="cook_to_do";
    const std::string barman_tb="barman_to_do";
    try
    {

        sql::Driver* driver = get_driver_instance();

        ok=true;
        std::unique_ptr<sql::Connection> con(
            driver->connect(url, user, pass)
        );
        con->setSchema(schema);
        
        sql::Driver* driver2=get_driver_instance();
        std::unique_ptr<sql::Connection> con2(
            driver2->connect(url, user, pass)
        );
        con2->setSchema(schema);
        std::map<int,std::string> choice={{1,cook_tb},{0,barman_tb}};
        int x;
        std::cout<<"what are you , a cook or a barman ? (1=cook) (0=barman) ";
        std::cin>>x;
        while(x<0||x>1){
            std::cout<<"you didn't enter a right key , press again ";
            std::cin>>x;
        }

        std::thread t1(showing,choice.at(x),con.get());
       std::thread t2(command_done,choice.at(x),con2.get());
       t1.join();
       t2.join();
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "MySQL error: " << e.what() << '\n';
        std::cerr << "Error code: " << e.getErrorCode() << '\n';
        std::cerr << "SQLState: " << e.getSQLState() << '\n';
    }

    return 0;
}