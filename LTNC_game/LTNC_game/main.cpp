/**
    Game of Blackjack
    You can play with one or two players
    each player creaates a account if they don't have one and if they do they can load their existing data
    Each player and the dealer are dealt two cards, the dealers first card is face down
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "SDL_function.hpp"
#include <sstream>
#include <iomanip>


using namespace std;

//khai báo
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const string WINDOW_TITLE = "Blackjack";
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1000;
SDL_Texture *background = nullptr, *menu = nullptr, *rule = nullptr, *chipset = nullptr;
void unload_SDL_and_Images();
vector<vector<SDL_Texture* > > fullDeck;
//Chất bài (bích cơ rô tép)
enum card_suit{
    S, H, D, C
};

//Mức bet tối thiểu (khai báo global để mỗi lần sử dụng ở trong 1 function không phải khởi tạo lại)
int MIN_BET = 5; //Giá trị bet tối thiểu là 5

//Structures
//struct acount lưu trữ dự liệu về người chơi
struct account{ //account structure. This will store all the player information
    string name; //tên người chơi
    string email; //email
    string username; //username của người chơi
    string password; //password của người chơi

    //tiền của người chơi
    //trường hợp người chơi chơi trở lại tiền vẫn sẽ được lưu trữ
    int money;

    //các thông số người chơi
    int wins; //số ván thắng
    int total_played; //số ván đã chơi
    double win_percentage; //tỉ lệ thắng
};

//thông tin lá bài
struct card{
    int value; //giá trị số của lá bài
    card_suit suit; //chất của lá bài
    bool up; //kiểm tra lá bài úp hay ngửa (sử dụng cho dealer vì 1 lá của dealer sẽ phải để úp)
};

//struct thông tin về người chơi trong game
//có bao gồm cả dealer
struct player{
    account info; //thông tin account của người chơi
    vector<card> hand; //những lá trên tay người chơi
    int bet; //mức bet trong ván của người chơi
    int insurance_bet; //mức bet nếu người chơi sử dụng bảo hiểm
};

//prototypes
card deal();
card hitMe();
int score(vector<card>);
bool hasAce(vector<card>);
void printCards(vector<card>);
char printSuit(card);
void doubleDown(player, player&);
int winner(player, player&);
void payout(player, player&);
void split(player&, player&);
void bet(player&);
void insurance(vector<player>&);
account load();
void save(player);
account create();
void play(vector<player>&);
void clear(vector<card>&);
void dealer_play(player&);
void displayMenu();
void displayRules();
void load_SDL_and_Imanges();
void unload_SDL_and_Imanges();
int getPlayerChoicePlayOrRule();
void let_playler_choose_to_play_or_rule();

int main(){
    initSDL(window, renderer, WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT);
    load_SDL_and_Imanges();
    
    SDL_RenderClear(renderer);
    renderTexture(menu, renderer, 0, 0, 1600, 1000);
    SDL_RenderPresent(renderer);
    
    let_playler_choose_to_play_or_rule();
    
    
    
    //tạo 1 nộ rand số ngẫu nhiên để đoạn sau sử dụng hàm rand()
    unsigned int time_seed = time(0);
    srand(time_seed);

    player user; //khởi tạo người chơi
    player dealer; //khởi tạo dealer
    vector<player> players; //vector chứa danh sách người chơi
    char input; //sử dụng cho input

    dealer.info.username = "Dealer"; //setup tên dealer
    players.push_back(dealer); //đẩy dealer vào vector chứa danh sách người chơi
    int playerChoice; // lựa chọn của người chơi

    do{
        displayMenu(); //hiện thị menu
        cin >> playerChoice; //cho người chơi nhập lựa chọn

        if(playerChoice == 1){ //người chơi chọn đọc luật chơi
            displayRules(); //hiển thị luật chơi
        }
        else if(playerChoice == 2){ //người chơi chọn chơi
                cout << "How many players(1 or 2)" << endl; //hỏi số lượng người chơi
                //nhập số người chơi
                int numPlayers;
                cin >> numPlayers;

                for(int i = 0;i< numPlayers;i++){ //cycles through the players
                    cout << "Are you a new player(N) or existing player(E)?" << endl; //hỏi xem người chơi đã có tài khoản chưa
                    cin >> input; //người chơi nhập lựa chọn xem đã có tài khoản hay chưa

                    if(input == 'N' || input == 'n'){ //nếu người chơi chưa có tài khoản
                        user.info = create(); //gọi hàm khởi tạo tài khoản
                        save(user); //lưu thông tin người chơi
                        players.push_back(user); //thêm người chơi vào vector lưu danh sách thông tin người chơi
                    }
                    else if(input == 'E' || input == 'e'){ //nếu người chơi đã có tài khoản
                        user.info = load(); //load tài khoản của người chơi
                        players.push_back(user); //đẩy vào vector danh sách các người chơi
                    }

                }
        }
        else{ //nếu người chơi không chọn theo hướng dẫn
            cout << "Please enter 1 or 2" << endl; //yêu cầu người chơi chọn lại
            playerChoice = 0; //reset lại playerChoice về 0
            cin.ignore(); //reset bộ nhớ đệm
        }
    }while(playerChoice != 2); //chạy vòng lặp cho đến khi người chơi chọn play

    bool continuePlay = false; //đánh dấu xem có phải ván đầu tiên người chơi chơi không
    int size = players.size(); //số người chơi + 1 (dealer)
    do{
        for(int i =1;i<players.size();i++){
            if(players[i].info.money < 5){ //kiểm tra xem người chơi còn đủ tiền không
                cout << players[i].info.username << " is currently out of money. Would you like to add some, yes(Y) or (N)?" << endl; //thông báo cho người chơi họ không còn đủ tiền để chơi
                cin >> input; //cho người chơi lựa chọn có nạp thêm tiền không
                if(input == 'Y' || input == 'y'){ //nếu họ chọn nạp thêm tiền
                    cout << "How much would you like to add?" << endl;
                    int addedMoney;
                    cin >> addedMoney;
                    players[i].info.money += addedMoney;
//                    cin >> players[i].info.money; //takes it in
                }
                else{ //nếu người chơi không nạp thêm tiền
                    save(players[i]); //lưu thông tin người chơi
                    players.erase(players.begin() + (i)); //xoá người chơi ra khỏi ván
                    i--;

                }
            }

            if(continuePlay && players.size() > 1){//if they have played at least once
                cout << players[i].info.username << " Would you like to keep playing(P) or quit(Q)" << endl;
                cin >> input; //cho người chơi lựa chọn
                if(input == 'q' || input == 'Q'){ //người chơi muốn thoát
                    save(players[i]); //lưu thông tin người chơi
                    players.erase(players.begin() + (i)); //xoá người chơi ra khỏi ván
                    i--;
                }
            }
        }
        play(players); //chơi thôi
        continuePlay = true; //người chơi đã chơi xong 1 ván
    }while(players.size() > 1);
    //!(input == 'q' || input == 'Q')

    return 0;
}


void displayRules(){
    cout << "The rules of blackjack are fairly simple and your only opponent in the game is the dealer. Each player is dealt a two cards face up while the dealer only has one card face up. The goal is to have your two cards added total to be higher than the dealer’s two cards and under 21. If you go over 21 you “bust”, or lose, or if the dealers two cards added together are higher than yours you also lose.     If your two card total is equal to the dealers then it is a “stand-off” or a tie. Cards values are usually face value, 2 is 2 and 10 is 10, while all face cards, jack/queen/king, are also 10. The only exception to this rule are Aces, they can take on the value of 1 or 11. To get your two cards total you simply add your two cards together. If you have any combination of an Ace or any card that is 10 points then you have what is called blackjack, 21 pts. Getting blackjack means you get paid more if you win. With all of that being said if you’re not satisfied with your two card total then you can take extra cards, called taking a hit, and for each hit you get you get one more card. The dealer does this as well but has a strict set of rules to follow on whether or not to take a hit. If a dealer’s hand is less than or equal to 16 or a “soft” 17, meaning they have an ace and a 6, they must take a hit. If it’s 17 or higher the dealer must stand.  So now that we know the dealer rules there are a few options for users.  As I stated earlier you can take hits to increase your card total. You may also split your cards and double down. Splitting can be done when your first two cards are of equal value and can only be split from the original hand, split hands cannot be split, the bet has to be equal or greater than the original bet for each hand. For each time you split you will receive and additional card for that hand and then you play like regular blackjack.  Users may also double down which consists of a user placing another bet of equal or lesser value when their first two cards total is equal to 9, 10, or 11 without aces. After doubling down you will only get one additional card. Some of you may have realized that if the dealer gets a blackjack or 21 you pretty much always lose, unless you yourself have blackjack. There is a way around this and it’s called insurance. If the dealer is dealt an ace face up then the dealer will ask if you want to take out insurance, equal to half of your original bet, to insure your hand if the dealer has blackjack and only when he has blackjack and helps insure you don’t lose money if he does have blackjack, insurance pays 2 to 1 so your insurance bet will cover the loss of your hand if you bet half." << endl;
}

//hiển thị menu
void displayMenu(){
    cout << "Welcome to the game of Blackjack!" << endl;
    cout << "Please select an option below." << endl;
    cout << "1) Rules" << endl;
    cout << "2) Play" << endl;
}
/**
    Rules for the dealer. They hit on everything up to 17 including a soft 17
*/
void dealer_play(player &dealer){
    if((score(dealer.hand) < 17) || (score(dealer.hand) == 17 && hasAce(dealer.hand))){ //dealer bốc thêm bài nếu dưới 17 điểm
        dealer.hand.push_back(hitMe()); //dealer gets a card
    }
}

void play(vector<player> &players){
    char input; //dùng cho các loại input

    for(int i =1;i<players.size();i++){ //cho người chơi lần lượt bet
        bet(players[i]); //players bet here
        cout << "Money: " << players[i].info.money << setw(10) << "Bet: " << players[i].bet << endl;
    }
    
    //chia lá đầu tiên cho dealer và úp lá đó
    players[0].hand.push_back(deal());
    players[0].hand[0].up = false;
    
    //chia lần lượt mỗi người 1 lá cho đến khi đủ 2 lá mỗi người
    for(int i = 1;i< (players.size()*2);i++){
        players[(i%players.size())].hand.push_back(deal());
//        if((i%players.size()) == 0 && (i%2) == 0){ //úp lá đầu tiên của dealer
//            players[(i%players.size())].hand[(i%2)].up = false;
//        }
    }
    
    
//    //test insurance bet
//    for(int i = 1;i< (players.size()*2 );i++){
//        if ((i%players.size()) != 0) {
//            players[(i%players.size())].hand.push_back(deal());
//        }
//
//    }
//    card anAceCard;
//    anAceCard.suit = S;
//    anAceCard.value = 1;
//    anAceCard.up = true;
//    players[0].hand.push_back(anAceCard);
    
    //in số điểm của mỗi người chơi
    for(int i=1;i<players.size();i++){
        cout << players[i].info.username << " has: " << score(players[i].hand) << setw(10) << endl;
    }

    //hiện bài người chơi
    for(int i =0;i<players.size();i++){
        cout << players[i].info.username << " Cards:" << endl;
        printCards(players[i].hand);
    }
    
    //chơi game
    bool cont = true;
    for(int i = 1;i<players.size();i++){
        do{
            if(players[0].hand[1].value == 1 && cont){ // nếu lá ngửa của dealer là 1 lá át
                insurance(players);
                if(score(players[0].hand) == 21){
                    players[0].hand[0].up = true;
                    printCards(players[0].hand);
                    
                    for(int i =1;i<players.size();i++){
                        payout(players[0],players[i]);
                    }
                    input = 'S';
                }
                cont = false;
            }
            if(players[0].hand[1].value >= 10 && cont){
                if(score(players[0].hand) == 21){
                    players[0].hand[0].up = true;
                    printCards(players[0].hand);

                    for(int i =1;i<players.size();i++){
                        payout(players[0],players[i]);
                    }
                    input = 'S';
                }
                cont = false;
            }

            
            if(score(players[0].hand) <= 21){

                    if(((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || players[i].hand[0].value == players[i].hand[1].value) && players[i].hand.size() == 2  && score(players[i].hand) == 10){ //2 lá giống nhau mà có tổng điểm bằng 10 thì được split hoặc double down
                        cout << players[i].info.username << " score: " << score(players[i].hand) << endl;
                        cout << "Would you like to Double Down(D), split(L),take a hit(H), or stay(S), default is to take a stay?" << endl;
                    }
                    else if(((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || (players[i].hand[0].value == players[i].hand[1].value)) && players[i].hand.size() == 2){ //2 lá giống nhau được split
                        cout << players[i].info.username << " score: " << score(players[i].hand) << endl;
                        cout << "Would you like to split(L) your cards, take a hit(H), or stay(S), default is to take a stay?" << endl;
                    }
                    else if(players[i].hand.size() == 2 && score(players[i].hand) >= 9 && score(players[i].hand) <= 11 && !(hasAce(players[i].hand))){ //tổng điểm từ 9 - 11 thì được double down
                        cout << players[i].info.username << " score: " << score(players[i].hand) << endl;
                        cout << "Would you like to Double Down(D), take a hit(H), or stay(S), default is to take a stay?" << endl;
                    }
                    else{//không có gì đặc biệt thì chỉ được hit hoặc stay
                        cout << players[i].info.username << " score: " << score(players[i].hand) << endl;
                        cout << "Hit(H) or Stay(S), default is to take a stay?";
                    }
                    cin >> input; //lấy lựa chọn
                    switch(input){
                        //split
                        case 'L':
                        case 'l':
                        {
                            split(players[0], players[i]);
                            printCards(players[i].hand);
                            break;
                        }
                        // double down
                        case 'D':
                        case 'd':
                        {
                            doubleDown(players[0], players[i]);
                            input = 'S';
                            break;
                        }
                        // hit
                        case 'H':
                        case 'h':
                        {
                            players[i].hand.push_back(hitMe());
                            printCards(players[i].hand);
                            cout << players[i].info.username << " score is now " << score(players[i].hand) << endl;
                            break;
                        }
                            
                        default:
                            input = 's';
                    }
                    if(score(players[i].hand) > 21){
                        input = 'S';
                    }
            }
        }while(!(input == 'S' || input == 's')); //tiếp tục vòng lặp cho đến khi người chơi chọn stay
    }

    dealer_play(players[0]); //lượt chơi của dealer

    players[0].hand[0].up = true; //lật bài dealer
    
    //in bài và điểm mọi người
    for(int i =0;i<players.size();i++){
        cout << players[i].info.username << " score: " << score(players[i].hand) << " Cards: ";
        printCards(players[i].hand);
    }


    for(int i =1;i<players.size();i++){
        if(score(players[i].hand) > 21){
            cout << "You busted! ";
        }
        int win = winner(players[0], players[i]);
        if(win == 1){
            players[i].info.wins += 1;
        }
        payout(players[0],players[i]);
        clear(players[i].hand);
        players[i].info.total_played+=1;
    }

    clear(players[0].hand);
}

//bỏ hết bài trên tay
void clear(vector<card> &hand){
    hand.clear();
}

//tạo account cho người chơi mới
account create(){
    account user; //khởi tạo 1 account mới
    cout << "What is the username you'd like to use?" << endl;
    
    //khai báo thông tin file dữ liệu người chơi
    fstream input; //input
    string filename; //tên file

    do{
        cin >> user.username; //nhập usermame
        filename = user.username + ".dat"; //thêm đuôi cho tên file
        input.open(filename); //thử mở file có tên username người chơi đã nhập
        //kiểm tra xem username có bị trùng không
        if(!input.fail()){
            cout << "This username is already taken. Please choose another one." << endl;
        }
    }while(!input.fail()); //chạy vòng lặp đến khi người chơi chọn được tên username không bị trùng

    cout << "Please enter a password." << endl; //nhập password
    cin.ignore();//xoá bộ nhớ đệm
    getline(cin, user.password); //cho người chơi nhập password (sư dụng getline vì password được phép có dấy cách)
    cin.ignore(); //xoá bộ nhớ đệm

    cout << "Please enter your name." << endl; //hỏi tên người chơi
    getline(cin, user.name); //nhập tên người chơi (sử dụng getline vì tên có thể có dấu cách)

    cout << "Please enter your email address." << endl; //hỏi email address
    cin >> user.email; //cho người chơi nhập tên

    cout << "Please enter how much money you'd like to deposit" << endl; //hỏi lượng tiền người chơi muốn buy in
    cin >> user.money; //cho người chơi nhập số tiền muốn buy in

    //set tất cả các thông số người chơi về 0
    user.total_played = 0;
    user.wins = 0;
    user.win_percentage = 0;

    input.close(); //đóng file
    return user; //trả về thông tin 1 người chơi thuộc struct account
}

//lưu thông tin account
void save(player user){
    ofstream output; //output stream
    string filename = user.info.username + ".dat"; //so we know what file to save to

    output.open(filename, ios::out); //we want to erase all previous data

    if(!output){//if we can't open output
        cerr << "Output file will not open and data could not be saved. " << endl;
    }

    double percent = 0; //for the win percentage
    if(user.info.total_played == 0){
        percent = 0; //so we don't divide by zero
    }
    else{
        percent = ((double)user.info.wins/user.info.total_played)*100; //win percentage
    }
    output << user.info.username << endl << user.info.password << endl << user.info.name << endl << user.info.email << endl << user.info.money << endl << user.info.total_played << endl << user.info.wins << endl << setprecision (3) << percent; //writes the data to the file

    output.close(); //closes the output stream
}

//load thông tin người chơi cũ
account load(){
    string username, password;
    string filename;
    account user;
    ifstream input;

    //cho người chơi đăng nhập
    cout << "What is your username" << endl;
    cin >> username;
    filename = username + ".dat";
    input.open(filename);

    while (input.fail()) {
        cout << "Your username is incorrect, please re-enter it. " << endl;
        cin >> username;
        filename = username + ".dat";
        input.open(filename);
    }
    

    getline(input, user.username); //lấy username từ file data
    getline(input, user.password); //lấy password từ file data

    
    cout << "What is your password? " << endl;
    cin >> password;
    
    while (!(password == user.password)) {
        cout << "Wrong password! Please re-enter password. " << endl;
        cin >> password;
    }
    
 
    getline(input, user.name);
    getline(input, user.email);
    input >> user.money;
    input >> user.total_played;
    input >> user.wins;
    input >> user.win_percentage;
    
    input.close(); //đóng file
    return user; //trả về user
}
/**
    if the dealer is displaying an Ace it asks all the players if they want to take out insurance
*/
void insurance(vector<player> &players){
    int size = players.size();
    
    for(int i=1;i<size;i++){
        int bet;
        players[i].insurance_bet = 0;
        if((players[i].info.money-(players[i].bet/2)) >=0){ //kiểm tra xem còn tiền để có thể insurance bet không
            do{
                cin.ignore();
                cout << "How much would you like to bet for insurance, up to " << (players[i].bet/2) << " " << players[i].info.username << " ?(0 if you don't want to take out insurance)" << endl;
                cin >> bet; //takes it in
            }while(!(bet <= (players[i].bet/2) && bet >=0));

            if(bet != 0){
                players[i].insurance_bet = bet;
                players[i].info.money-=bet;
            }
        }
    }

    if(score(players[0].hand) == 21){ // nếu người chơi thắng
        for(int i = 1; i<size;i++){
            if(players[i].insurance_bet != 0){
                players[i].info.money+=(players[i].insurance_bet*2);
                players[i].insurance_bet = 0;
            }
        }
    }
    else{ // nếu người chơi thua
        for(int i = 1; i<size;i++){
            if(players[i].insurance_bet != 0){
                players[i].insurance_bet = 0;
            }
        }
    }
}

/**
   Asks the user what they would like to bet and returns it
*/
void bet(player &user){
    int bet;
    do{
        cout << user.info.username << endl; //in tên người bet
        cout << "How much would you like to bet? (Must be greater than the " << MIN_BET <<  " and less than " << user.info.money << ")" << endl;
        cin >> bet; //nhập mực bet
    }while(!(bet >= MIN_BET && bet <= user.info.money));
    user.info.money -= bet; //trừ tiền đi
    user.bet = bet; //set mức bet
}

void split(player &dealer, player &user){
    player split; //tạo hand mới để split
    vector<player> players;
    players.push_back(dealer);
    
    split.bet = user.bet;
    user.info.money -= user.bet;
    split.hand.push_back(user.hand[0]);
    split.hand.push_back(deal());
    user.hand[0] = deal(); //bốc cho người chơi lá mới
    split.info.username = user.info.username;
    players.push_back(split);

    printCards(players[1].hand);

    char input;
    do{
        for(int i =1;i<players.size();i++){
            if(score(players[i].hand) > 21){
                input = 'S';
            }
            else{
                cout << "Hit(H) or Stay(S):";
                cin >> input;
            }
            
            if(input == 'H' || input == 'h'){
                players[i].hand.push_back(hitMe());
                printCards(players[i].hand);
                cout << players[i].info.username << " score is now " << score(players[i].hand) << endl;
            }
        }
    }while(!(input == 'S' || input == 's'));

    dealer_play(players[0]);

   
    for(int i =1;i<players.size();i++){
        cout << players[i].info.username << " score: " << score(players[i].hand) << " Cards: " << endl;
        printCards(players[i].hand);
    }

    for(int i =1;i<players.size();i++){
        if(score(players[i].hand) > 21){
            cout << "You busted!";
        }
        payout(players[0],players[i]);
    }
}

void payout(player dealer, player &user){
    if(winner(dealer, user) == 1){ //người chơi thắng
        if(score(user.hand) == 21 && hasAce(user.hand) && user.hand.size() == 2){ //blackjack an 3/2
            user.info.money += ((user.bet*3)/2);
            user.bet = 0;
            cout << user.info.username << " won!" << endl;
        }
        else{
            user.info.money+= (user.bet*2);
            user.bet = 0;
            cout << user.info.username << " won!" << endl;
        }
    }
    else if (winner(dealer, user) == 0){ //hoà
        user.info.money+= user.bet;
        user.bet = 0;
        cout << user.info.username << " tied!" << endl;
    }
    else{ //thua
        user.bet = 0;
        cout <<user.info.username << " lost!" << endl;
    }
}

int winner(player dealer, player &user){
    if(score(dealer.hand) == score(user.hand)){
        return 0;
    }
    else if(((score(dealer.hand) < score(user.hand)) && (score(user.hand) <= 21)) || (score(dealer.hand) > 21 && score(user.hand) <= 21)){
        return 1;
    }
    else{
        return -1;
    }
}


void doubleDown(player dealer, player &user){
    int bet; //so we can store the new bet
    do{
        cout << "How much would you like to bet? (Must be greater than the " << MIN_BET << " and less than "<< user.info.money <<")" << endl; //asks how much they want to bet
        cin >> bet; //takes it in
    }while(!(bet > MIN_BET && bet <= user.bet && bet <= user.info.money)); //we repeat this until they get it right

    user.bet+=bet; //add the new bet to the original
    user.info.money-=bet; //takes out their double down bet from the money
    user.hand.push_back(deal()); //gives the user one more card
    payout(dealer, user); //pays out the player based on if he/she won
}
/**
Returns the Character version of the suit
Fairly self-explanatory...takes in a number between 0-4 and returns the suit based off that number
*/
char printSuit(card new_card){
    switch(new_card.suit){
        case 0:
            return 'S';
        case 1:
            return 'H';
        case 2:
            return 'D';
        case 3:
            return 'C';
    }
}

/**
Prints the cards to the screen
*/
void printCards(vector<card> hand){
    const string CARD_VALUES[14] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "X"};

    for(int i=0;i<hand.size();i++){
        /**cout << "----------" <<  endl
            << "|" << ((hand[i].up)?(hand[i].value):('X')) << ((hand[i].up)?((printSuit(hand[i]))):('X')) << setw(6) << "|" << endl
                << "|" << setw(8) << "|" << endl
                << "|" << setw(8) << "|" << endl
                << "|" << setw(8) << "|" << endl
                << "|" << setw(6) << ((hand[i].up)?(hand[i].value):('X')) <<  ((hand[i].up)?(printSuit(hand[i])):('X')) << "|" << endl
                << "----------" << endl;*/
        if(hand[i].up){ //nếu bài ngửa
            cout << CARD_VALUES[(hand[i].value-1)] << printSuit(hand[i]) << " ";
        }
        else{ //nếu bài úp thì in XX
            cout << CARD_VALUES[13] << CARD_VALUES[13] << " ";
        }
    }
    cout << endl;
}


//check xem có át không
bool hasAce(vector<card> hand){
    for(int i =0;i<hand.size();i++){
        if(hand[i].value == 1){
            return true;
        }
    }

    return false;
}

// tính tổng điểm trên tay
int score(vector<card> hand){
    int total = 0; //tổng điểm
    for(int i = 0; i<hand.size();i++){
        if(hand[i].value >= 10){ //tính điểm với những lá đầu người hoặc lá 10
            total+=10;
        }
        else{
            total += hand[i].value; //tính điểm với những lá bình thường
        }
    }

    if(hasAce(hand) && total <= 11){ //nếu có át mà bài chưa cộng vào chưa quá 21 thì đổi át thành 11 điểm
        total+=10;
    }

    return total;
}

//hàm bốc thêm bài
card hitMe(){
    return deal();
}


//hàm chia 1 lá bài
card deal(){
    card new_card;

    new_card.value = 1 + rand() % 13; //random số
    //random chất
    int suit = rand() % 4;
    switch(suit){
        case 0:
            new_card.suit = S;
            break;
        case 1:
            new_card.suit = H;
            break;
        case 2:
            new_card.suit = D;
            break;
        case 3:
            new_card.suit = C;
            break;
    }

    new_card.up = true;

    return new_card;
}

//void load_SDL_and_Imanges() {
//    background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/card_deck.png", renderer);
//}

int getPlayerChoicePlayOrRule() {
    int choice = -1;
    SDL_Event e;
    while (choice <= 0) {
        SDL_Delay(10);
        if (SDL_WaitEvent(&e) == 0) continue;
        if ((e.type == SDL_QUIT) || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            unload_SDL_and_Images();
            exit(1);
        }
        if ((e.type == SDL_MOUSEBUTTONDOWN)) {
            if (e.button.x >= 178 && e.button.x <= 384 && e.button.y >= 598 && e.button.y <= 694) {
                choice = 1;
                cout << "Rule" << endl;
                return choice;
                break;
            }
            if (e.button.x >= 178 && e.button.x <= 384 && e.button.y >= 478 && e.button.y <= 697) {
                choice = 2;
                cout << "Play" << endl;
                return choice;
            }
        }
    }
    return choice;
}

void load_SDL_and_Imanges() {
    menu = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/menu.png", renderer);
    rule = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/rule.PNG", renderer);
    background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/play_background.png", renderer);
    fullDeck = loadFullDeck(renderer);
    chipset = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/chipset.png", renderer);
    
}

void unload_SDL_and_Images() {
    SDL_DestroyTexture(menu);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(rule);
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            SDL_DestroyTexture(fullDeck[i][j]);
        }
    }
}

void let_playler_choose_to_play_or_rule() {
    int choice = getPlayerChoicePlayOrRule();
    while (choice != 2) {
        if (choice == 1) {
            SDL_Delay(150);
            SDL_RenderClear(renderer);
            renderTexture(rule, renderer, 0, 0, 1600, 1000);
            SDL_RenderPresent(renderer);
            SDL_Event e;
            bool c = true;
            while (c) {
                SDL_Delay(10);
                if (SDL_WaitEvent(&e) == 0) continue;
                if ((e.type == SDL_QUIT) || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                    unload_SDL_and_Images();
                    exit(1);
                }
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    if (e.button.x <= 134 && e.button.y <= 101) {
                        SDL_Delay(150);
                        SDL_RenderClear(renderer);
                        renderTexture(menu, renderer, 0, 0);
                        SDL_RenderPresent(renderer);
                        c = false;
                        break;
                    }
                }
            }
        }
        choice = getPlayerChoicePlayOrRule();
    }
    SDL_Delay(150);
    SDL_RenderClear(renderer);
    renderTexture(background, renderer, 0, 0);
    renderTexture(chipset, renderer, 500, 300);
    SDL_RenderPresent(renderer);
}


