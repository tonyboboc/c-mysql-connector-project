
#include <mysql/jdbc.h>  
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <array>
#include <thread>
#include <chrono>
#include <mutex>
        const std::string url  = "tcp://127.0.0.1:3306"; 
        const std::string user = "mysql";
        const std::string pass = "mysql1234";
        const std::string schema = "mydb";
void show_menu(const std::string & menu,sql::Connection *con){
        sql::Statement *stmt;
        sql::ResultSet *res;
        std::string sql_statement="Select * from "+menu+";";
        stmt=con->createStatement();
        res=stmt->executeQuery(sql_statement);
        while(res->next()){
            std::cout<<"id= "<<res->getInt(1)<<" ";
            std::cout<<" | name of item : "<<res->getString(2);
            std::cout<<" | price : "<<res->getDouble(3);
            std::cout<<std::endl;
        }
        delete res;
        delete stmt;
} 
std::mutex mtx;
void wainting_for_response(int x,const std::string table){
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();
std::unique_ptr<sql::Connection> con(
            driver->connect(url, user, pass)
        );
        con->setSchema(schema);
    std::string sql_statement="Select * from "+table+" WHERE id= "+std::to_string(x) + " ;";

    sql::Statement *stmt;

    stmt=con->createStatement();
    while (true) {
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql_statement));
        if (!res->next()) break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    delete stmt;
    mtx.lock();
    std::cout<<"Your order was finished\n";
    mtx.unlock();
}
std::thread adding_to_table(std::string table,sql::Connection *con,const  std::vector<std::string> & v){
        std::string query = "SELECT MAX(id) FROM " + table + ";";
         sql::PreparedStatement* prep = con->prepareStatement(query);
        sql::ResultSet* res = prep->executeQuery();
         int id = 0;
    if (res->next()) {
        id = res->getInt(1)+1;
    }
    else{
        id=1;
    }
    delete res;
    delete prep;
    query="INSERT INTO "+table +" (id,name) VAlUES (?,?);";
    prep=con->prepareStatement(query);
    for(const std::string & item:v){
        prep->setInt(1,id);
        prep->setString(2,item);
        prep->execute();
    }
    delete prep;
    std::thread t (wainting_for_response,id,table);
    return t;

}  
std::thread placing_order(std::string menu,std::string table ,sql::Connection *con){
    std::cout<<"do you want the menu 1(yes) 0(no) ";
    int choice;
    std::vector<std::string> to_be_ordered;
    std::cin>>choice;
    if(choice==1){
        show_menu(menu, con);
    }
    std::cout<<"chose what you want to order (based on id) (0 means exit) ";
    choice =-1;
    sql::PreparedStatement *prep_stmt;
    std::string string_statement="SELECT name FROM " + menu+ " WHERE id = ?;";
    prep_stmt=con->prepareStatement(string_statement) ;
    sql::ResultSet* res;
    while(choice !=0){
        std::cin>>choice;
        if(choice==0){
            break;
        }
        prep_stmt->setInt(1,choice);
        res=prep_stmt->executeQuery();
        if(!res->next()){
            std::cout<<"Invalid Item";
        }
        else{
            std::cout<<res->getString(1)<<" added to your choices ";
            to_be_ordered.push_back(res->getString(1));
        } 
         delete res;
    }
    delete prep_stmt;
    if(!to_be_ordered.empty()){
        return adding_to_table(table,con,to_be_ordered);
    }
    else{
        return std::thread(); 
    }
    
}
std::thread order(const std::string & bar_menu,const std::string & food_menu,sql::Connection *con,const std::string & cook_tb, const std::string & barman_tb){
    int what_type;
    std::map<int , std::array<std::string, 2>> choice;
    choice[0]={bar_menu,barman_tb};
    choice[1]={food_menu,cook_tb};
    std::cout<<"what do you want to order, food or drink ? (1=food 0=drink) ";
    std::cin>>what_type;
    if(what_type<0||what_type>1){
        std::cout<<"you didn't choose what to order ";
        std::cin>>what_type;
    }
    return placing_order(choice[what_type][0],choice[what_type][1],con);
    
}

int main() {
    try {

        const std::string bar_menu="menu_bar";
        const std::string food_menu="menu_food";
        const std::string cook_tb="cook_to_do";
        const std::string barman_tb="barman_to_do";
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();

        
        std::unique_ptr<sql::Connection> con(
            driver->connect(url, user, pass)
        );
        con->setSchema(schema);
        show_menu(bar_menu,con.get());
        std::cout<<"now showing food menu\n";
        show_menu(food_menu,con.get());
        int x;
        std::cout<<"do you want to order (1 yes) (0 no) ";
        std::cin>>x;
        std::vector<std::thread> orders;
        while(x!=0){
            if(x!=1){
                std::cout<<"you didn't chose yes or no , enter again what you want ";
                std::cin>>x;
                continue;
            }
            std::thread t=order(bar_menu,food_menu,con.get(),cook_tb,barman_tb);
            std::cout<<"your order is complete ,wait for it to be done \n";
            orders.emplace_back(std::move(t));
            std::cout<<"do you want to order something else ? ";
            std::cin>>x;
        }
        std::cout<<"your ordering is done, wait for your commands\n";
        for(auto & t :orders){
            if(t.joinable()){
            t.join();
        }
        }
        

    } catch (const sql::SQLException &e) {
        std::cerr << "MySQL error: " << e.what()
                  << " (code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << ")\n";
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "STD exception: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception!\n";
        return 1;
    }

    return 0;
}
