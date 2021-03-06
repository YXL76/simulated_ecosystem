﻿#include "pch.h"

#include <map>
#include <array>
#include <queue>
#include <chrono>
#include <random>
#include <string>
#include <iostream>
#include <windows.h>

using std::ios;
using std::map;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::array;
using std::queue;
using std::string;
using std::chrono::system_clock;
using std::default_random_engine;
using std::bernoulli_distribution;
using std::uniform_int_distribution;

constexpr auto kWidth = 118; //页面宽度（默认118）
constexpr auto kHeight = 32; //页面高度（默认32）
constexpr auto kDuration = 320; //最大周期（默认320）
constexpr auto kSleepTime = 0; //休眠时间（默认200）
constexpr auto kMinFish = 3; //初始最少大鱼数量（默认3）
constexpr auto kMaxFish = 4; //初始最多大鱼数量（默认4）
constexpr auto kMinAquatic = 2; //初始水草最小长度（默认2）
constexpr auto kMaxAquatic = 4; //初始水草最大长度（默认4）
constexpr auto kAquaticDevelopTime = 28; //水草生长周期（默认28）
constexpr auto kAquaticDeliverTime = 56; //水草繁衍周期（默认56）
constexpr auto kAquaticBirthRate = 0.4; //水草出生率（默认0.4）
constexpr auto kBigFishLifetime = 42; //大鱼寿命（默认42）
constexpr auto kBigFishDeliverTime = 18; //大鱼生育年龄（默认18）
constexpr auto kBigFishBirthRate = 0.2; //大鱼出生率（默认0.2）
constexpr auto kSmallFishLifetime = 24; //小鱼寿命（默认24）
constexpr auto kSmallFishDeliverTime = 5; //小鱼生育年龄（默认5）
constexpr auto kSmallFishBirthRate = 0.6; //小鱼出生率（默认0.6）

constexpr auto kBigFishEnergy = 100; //大鱼最大饥饿值（默认100）
constexpr auto kSmallFishEnergy = 60; //小鱼最大饥饿值（默认60）
constexpr auto kEnergyOfAquatic = 6; //水草能量（默认6）
constexpr auto kEnergyOfSmallFish = 24; //小鱼能量（默认24）
constexpr auto kMoveEnergyOfBigFish = 3; //大鱼移动能量（默认3）
constexpr auto kMoveEnergyOfSmallFish = 2; //小鱼移动能量（默认2）

constexpr array<int, 9> kXMove = {0, -2, 0, 2, 2, 2, 0, -2, -2};
constexpr array<int, 9> kYMove = {0, 2, 2, 2, 0, -2, -2, -2, 0};
const array<string, 5> kIcon = {"  ", "┃", "○", "●", "█"};

const auto kHOut = GetStdHandle(STD_OUTPUT_HANDLE);
const auto kSeed = system_clock::now().time_since_epoch().count();

default_random_engine generator(static_cast<unsigned>(kSeed));

bernoulli_distribution aquatic_existence(kAquaticBirthRate);
bernoulli_distribution big_fish_existence(kBigFishBirthRate);
bernoulli_distribution small_fish_existence(kSmallFishBirthRate);

uniform_int_distribution<> x_position(2, 57);
uniform_int_distribution<> y_position_big_fish(2, 4);
uniform_int_distribution<> y_position_small_fish(8, 10);
uniform_int_distribution<> move_direction(1, 8);
uniform_int_distribution<> fish_number(kMinFish, kMaxFish);
uniform_int_distribution<> aquatic_length(kMinAquatic, kMaxAquatic);

typedef pair<int, int> PosPair;

struct Write final
{
    string str;
} information;

array<Write, kHeight> write;

inline void GoToPosition(const PosPair & x);
inline void GoToPosition(const int & x, const int & y);

void WelcomePage();
void ExamplePage();
void SettingPage();
void EndingPage();
void SetBackground();
void SetConsole();
void Pause();
void PrintBackground();


class Ecosystem final
{
public:
    Ecosystem();
    ~Ecosystem();
    void ShowTime();
    void ShowData();
    bool Judge() const;

    void Initialization();
    void Develop();

private:
    struct Aquatic final
    {
        int age;
        int length;
    };

    struct Fish final
    {
        bool have_baby;
        int id;
        int age;
        int state;
        int step;
        int hunger;
        PosPair position;
        void Feed(const int x) { hunger += x; }
        bool IsHungry(const int x) const { return hunger <= x; }

        void Move(int & x, const int y)
        {
            position.first += kXMove[x];
            position.second += kYMove[x];
            if (position.first < 3) { position.first = kWidth - 4; }
            else if (position.first > kWidth - 4) { position.first = 2; }
            hunger -= y;
        }

        Fish GiveBirth(const int x)
        {
            have_baby = true;
            return {false, 0, 0, 0, 0, x, position};
        }
    };

    int world_time_ = -1;

    int aquatic_number_ = 0;
    int big_fish_number_ = 0;
    int small_fish_number_ = 0;

    array<Aquatic, kWidth> aquatic_{};
    map<int, Fish> big_fish_;
    map<int, Fish> small_fish_;

    array<bool, kWidth> is_aquatic_{};
    array<array<int, kHeight>, kWidth> is_big_fish_{};
    array<array<int, kHeight>, kWidth> is_small_fish_{};

    queue<int> reduce_temporary_;
    queue<Fish> increase_temporary_;

    static bool WithinBoundary(PosPair & x);
    static bool WithinBoundary(const int & x, const int & y);
    static int JudgeDirection(const PosPair & x, const PosPair & y);
    static void Print(const string & str, const PosPair & position);

    void ReduceAquatic(int position);
    void IncreaseAquatic(Aquatic & x, int position);
    void DevelopAquatic();

    void ReduceBigFish(int x);
    void IncreaseBigFish(Fish x);
    void DevelopBigFish();
    void MoveBigFish(Fish & x, int destination);
    int SearchSmallFish(Fish & x);
    void HuntSmallFish(Fish & x);

    void ReduceSmallFish(int x);
    void IncreaseSmallFish(Fish x);
    void DevelopSmallFish();
    void MoveSmallFish(Fish & x, int destination);
    int SearchBigFish(Fish & x) const;
    void HuntAquatic(Fish & x);
} pond;

Ecosystem::Ecosystem()
= default;

Ecosystem::~Ecosystem()
= default;

void Ecosystem::ShowTime() { cout << "时间：" << world_time_++; }

void Ecosystem::ShowData()
{
    GoToPosition(4, 2);
    ShowTime();
    GoToPosition(4, 3);
    cout << kIcon[1] << " 水草：" << aquatic_number_ << kIcon[0];
    GoToPosition(4, 4);
    cout << kIcon[2] << "小鱼：" << small_fish_.size() << kIcon[0];
    GoToPosition(4, 5);
    cout << kIcon[3] << "大鱼：" << big_fish_.size() << kIcon[0];
}

bool Ecosystem::Judge() const
{
    if (world_time_ > kDuration)
    {
        information.str = "周期结束";
        return false;
    }
    if (!aquatic_number_)
    {
        information.str = "水草全部死亡";
        return false;
    }
    if (big_fish_.empty())
    {
        information.str = "大鱼全部死亡";
        return false;
    }
    if (small_fish_.empty())
    {
        information.str = "小鱼全部死亡";
        return false;
    }
    return true;
}

void Ecosystem::Initialization()
{
    SetConsoleTitle(TEXT("演示"));
    PrintBackground();
    for (auto i = 2; i < kWidth - 2; i += 2)
    {
        if (aquatic_existence(generator))
        {
            Aquatic temporary = {0, aquatic_length(generator)};
            IncreaseAquatic(temporary, i);
        }
    }
    const auto fish = fish_number(generator);
    for (auto i = 0; i < fish; ++i)
    {
        auto x = x_position(generator) << 1;
        auto y = y_position_big_fish(generator) << 1;
        const Fish temporary = {false, 0, 0, 0, 0, kBigFishEnergy, {x, y}};
        IncreaseBigFish(temporary);
    }
    for (auto i = 0; i < fish << 2; ++i)
    {
        auto x = x_position(generator) << 1;
        auto y = y_position_small_fish(generator) << 1;
        const Fish temporary = {false, 0, 0, 0, 0, kSmallFishEnergy, {x, y}};
        IncreaseSmallFish(temporary);
    }
}

void Ecosystem::Develop()
{
    DevelopAquatic();
    DevelopSmallFish();
    DevelopBigFish();
}

bool Ecosystem::WithinBoundary(PosPair & x)
{
    if (x.second < 2) return false;
    if (x.second >= kHeight - 2) return false;
    return true;
}

bool Ecosystem::WithinBoundary(const int & x, const int & y)
{
    if (x < 2) return false;
    if (x >= kWidth - 3) return false;
    if (y < 2) return false;
    return y < kHeight - 2;
}

int Ecosystem::JudgeDirection(const PosPair & x, const PosPair & y)
{
    if (x.first < y.first)
    {
        if (x.second < y.second) { return 7; }
        if (x.second == y.second) { return 8; }
        return 1;
    }
    if (x.first == y.first)
    {
        if (x.second < y.second) { return 6; }
        if (x.second == y.second) { return 0; }
        return 2;
    }
    if (x.second < y.second) { return 5; }
    if (x.second == y.second) { return 4; }
    return 3;
}

void Ecosystem::Print(const string & str, const PosPair & position)
{
    GoToPosition(position);
    cout << str;
}

void Ecosystem::ReduceAquatic(const int position)
{
    Print(kIcon[0], {position, kHeight - aquatic_[position].length - 2});
    --aquatic_[position].length;
    if (!aquatic_[position].length)
    {
        --aquatic_number_;
        is_aquatic_[position] = false;
    }
}

void Ecosystem::IncreaseAquatic(Aquatic & x, const int position)
{
    aquatic_[position] = x;
    ++aquatic_number_;
    is_aquatic_[position] = true;
}

void Ecosystem::DevelopAquatic()
{
    for (auto i = 2; i < kWidth - 2; i += 2)
    {
        if (!is_aquatic_[i]) continue;
        for (auto j = 0; j < aquatic_[i].length; ++j) { Print(kIcon[0], {i, kHeight - j - 2}); }
        ++aquatic_[i].age;
        if (aquatic_[i].age % kAquaticDevelopTime == 0)
        {
            if (aquatic_[i].length < kHeight - 6) { ++aquatic_[i].length; }
        }
        if (aquatic_[i].age % kAquaticDeliverTime == 0)
        {
            if (aquatic_existence(generator))
            {
                if (WithinBoundary(i + 2, 4) && !is_aquatic_[i + 2])
                {
                    Aquatic temporary = {0, 1};
                    IncreaseAquatic(temporary, i + 2);
                }
            }
        }
        for (auto j = 0; j < aquatic_[i].length; ++j) { Print(kIcon[1], {i, kHeight - j - 2}); }
    }
}

void Ecosystem::ReduceBigFish(const int x)
{
    const auto i = big_fish_.find(x);
    is_big_fish_[i->second.position.first][i->second.position.second] = 0;
    Print(kIcon[0], i->second.position);
    big_fish_.erase(i);
}

void Ecosystem::IncreaseBigFish(Fish x)
{
    x.id = ++big_fish_number_;
    big_fish_.insert({x.id, x});
    is_big_fish_[x.position.first][x.position.second] = x.id;
    Print(kIcon[3], x.position);
}

void Ecosystem::DevelopBigFish()
{
    for (auto & i : big_fish_)
    {
        ++i.second.age;
        if (i.second.hunger <= 0 || i.second.age > kBigFishLifetime)
        {
            reduce_temporary_.push(i.second.id);
            continue;
        }
        if (!i.second.have_baby && i.second.age > kBigFishDeliverTime && big_fish_existence(generator))
        {
            increase_temporary_.push(i.second.GiveBirth(kBigFishEnergy - kEnergyOfSmallFish));
            continue;
        }
        HuntSmallFish(i.second);
        MoveBigFish(i.second, SearchSmallFish(i.second));
    }
    while (!reduce_temporary_.empty())
    {
        ReduceBigFish(reduce_temporary_.front());
        reduce_temporary_.pop();
    }
    while (!increase_temporary_.empty())
    {
        IncreaseBigFish(increase_temporary_.front());
        increase_temporary_.pop();
    }
}

void Ecosystem::MoveBigFish(Fish & x, int destination)
{
    Print(kIcon[0], x.position);
    is_big_fish_[x.position.first][x.position.second] = 0;
    x.Move(destination, kMoveEnergyOfBigFish);
    Print(kIcon[3], x.position);
    is_big_fish_[x.position.first][x.position.second] = x.id;
}

int Ecosystem::SearchSmallFish(Fish & x)
{
    const auto il = x.position.first - 8;
    const auto ir = x.position.first + 8;
    const auto jl = x.position.second + 8;
    const auto jr = x.position.second - 8;
    for (auto i = il; i <= ir; i += 2)
    {
        for (auto j = jl; j >= jr; j -= 2)
        {
            PosPair y = {i, j};
            if (!WithinBoundary(y)) continue;
            if (is_small_fish_[i][j])
            {
                x.step = 0;
                return (x.state = JudgeDirection(y, x.position));
            }
        }
    }
    if (x.state && x.step <= 6)
    {
        PosPair y;
        y.first = x.position.first + kXMove[x.state];
        y.second = x.position.second + kYMove[x.state];
        if (WithinBoundary(y))
        {
            ++x.step;
            return x.state;
        }
    }
    x.step = 0;
    for (auto i = 0; i < 10; ++i)
    {
        const auto state = move_direction(generator);
        //if (state == 5 || state == 6 || state == 7) continue;
        PosPair y;
        y.first = x.position.first + kXMove[state];
        y.second = x.position.second + kYMove[state];
        if (WithinBoundary(y)) { return (x.state = state); }
    }
    return 0;
}

void Ecosystem::HuntSmallFish(Fish & x)
{
    for (auto i = 1; i <= 8; ++i)
    {
        if (x.IsHungry(kBigFishEnergy - kEnergyOfSmallFish))
        {
            PosPair y;
            y.first = x.position.first + kXMove[i];
            y.second = x.position.second + kYMove[i];
            if (WithinBoundary(y) && is_small_fish_[y.first][y.second])
            {
                x.Feed(kEnergyOfSmallFish);
                ReduceSmallFish(is_small_fish_[y.first][y.second]);
            }
        }
    }
}

void Ecosystem::ReduceSmallFish(const int x)
{
    const auto i = small_fish_.find(x);
    is_small_fish_[i->second.position.first][i->second.position.second] = 0;
    Print(kIcon[0], i->second.position);
    small_fish_.erase(i);
}

void Ecosystem::IncreaseSmallFish(Fish x)
{
    x.id = ++small_fish_number_;
    small_fish_.insert({x.id, x});
    is_small_fish_[x.position.first][x.position.second] = x.id;
    Print(kIcon[2], x.position);
}

void Ecosystem::DevelopSmallFish()
{
    for (auto & i : small_fish_)
    {
        ++(i.second.age);
        if (i.second.hunger <= 0 || i.second.age > kSmallFishLifetime)
        {
            reduce_temporary_.push(i.second.id);
            continue;
        }
        if (!i.second.have_baby && i.second.age > kSmallFishDeliverTime)
        {
            if (small_fish_existence(generator))
            {
                increase_temporary_.push(i.second.GiveBirth(kSmallFishEnergy - kEnergyOfAquatic));
                continue;
            }
        }
        HuntAquatic(i.second);
        MoveSmallFish(i.second, SearchBigFish(i.second));
    }
    while (!reduce_temporary_.empty())
    {
        ReduceSmallFish(reduce_temporary_.front());
        reduce_temporary_.pop();
    }
    while (!increase_temporary_.empty())
    {
        IncreaseSmallFish(increase_temporary_.front());
        increase_temporary_.pop();
    }
}

void Ecosystem::MoveSmallFish(Fish & x, int destination)
{
    Print(kIcon[0], x.position);
    is_small_fish_[x.position.first][x.position.second] = 0;
    x.Move(destination, kMoveEnergyOfSmallFish);
    Print(kIcon[2], x.position);
    is_small_fish_[x.position.first][x.position.second] = x.id;
}

int Ecosystem::SearchBigFish(Fish & x) const
{
    const auto il = x.position.first - 4;
    const auto ir = x.position.first + 4;
    const auto jl = x.position.second + 4;
    const auto jr = x.position.second - 4;
    for (auto i = il; i <= ir; i += 2)
    {
        for (auto j = jl; j >= jr; j -= 2)
        {
            PosPair y = {i, j};
            if (!WithinBoundary(y) || !is_big_fish_[i][j]) continue;
            PosPair z;
            z.first = (x.position.first << 1) - i;
            z.second = (x.position.second << 1) - j;
            if (!WithinBoundary(z)) continue;
            x.step = 0;
            return (x.state = JudgeDirection(z, x.position));
        }
    }
    if (x.IsHungry(kEnergyOfSmallFish - kEnergyOfAquatic))
    {
        for (auto i = il; i <= ir; i += 2)
        {
            for (auto j = jl; j >= jr; j -= 2)
            {
                PosPair y = {i, j};
                if (!WithinBoundary(y)) continue;
                if (!is_aquatic_[i]) continue;
                if (j >= kHeight - aquatic_[i].length - 1)
                {
                    x.step = 0;
                    return (x.state = JudgeDirection(y, x.position));
                }
            }
        }
    }
    if (x.state && x.step <= 6)
    {
        PosPair y;
        y.first = x.position.first + kXMove[x.state];
        y.second = x.position.second + kYMove[x.state];
        if (WithinBoundary(y))
        {
            ++x.step;
            return x.state;
        }
    }
    x.step = 0;
    for (auto i = 0; i < 10; ++i)
    {
        const auto state = move_direction(generator);
        //if (state == 6) continue;
        PosPair y;
        y.first = x.position.first + kXMove[state];
        y.second = x.position.second + kYMove[state];
        if (WithinBoundary(y)) { return (x.state = state); }
    }
    return 0;
}

void Ecosystem::HuntAquatic(Fish & x)
{
    for (auto i = 1; i <= 8; ++i)
    {
        if (x.IsHungry(kEnergyOfSmallFish - kEnergyOfAquatic))
        {
            PosPair y;
            y.first = x.position.first + kXMove[i];
            y.second = x.position.second + kYMove[i];
            if (!WithinBoundary(y) || !is_aquatic_[y.first]) continue;
            if (y.second >= kHeight - aquatic_[y.first].length - 1)
            {
                x.Feed(kEnergyOfAquatic);
                ReduceAquatic(y.first);
            }
        }
    }
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    SetBackground();
    SetConsole();
    WelcomePage();
    ExamplePage();
    SettingPage();
    pond.Initialization();
    do
    {
        Sleep(kSleepTime);
        pond.ShowData();
        pond.Develop();
        //Pause();
    }
    while (pond.Judge());
    EndingPage();
    system("CLS");
    return 0;
}

inline void GoToPosition(const PosPair & x)
{
    const COORD pos = {SHORT(x.first), SHORT(x.second)};
    SetConsoleCursorPosition(kHOut, pos);
}

inline void GoToPosition(const int & x, const int & y)
{
    const COORD pos = {SHORT(x), SHORT(y)};
    SetConsoleCursorPosition(kHOut, pos);
}

void WelcomePage()
{
    SetConsoleTitle(TEXT("欢迎"));
    PrintBackground();
    GoToPosition(2, 1);
    cout <<
        "  ..     ,   ..              ,                 -,                 -,                               .       -      ";
    GoToPosition(2, 2);
    cout <<
        "  $#    !@, .@#      $@,    ##   $#.      -.  ~@$                :@*                ..,,--~:;:     @$.    *@;     ";
    GoToPosition(2, 3);
    cout <<
        "  $# .~~=@; :@#~~-   $@, ,$:$@!  #@,     ,@@, ~@$                :@!         -$#@@@@@@@@@@@@@$    ;@#     -@@.    ";
    GoToPosition(2, 4);
    cout <<
        "  $# ;@@@@@@@@@@@@   $@, ~@*.@#  @@,     !@$  ~@$        .!******$@$*******, -@@@@@@@@#**;:~~,    $@,~=====@@$====";
    GoToPosition(2, 5);
    cout <<
        ",,$#,~;:=@! ;@#::~ ::#@!::@* *@! @@.     #@!--;@$~------ ,@@@@@@@@@@@@@@@@@~  -,..-#@#-   ~!     :@= :@@@@@@@@@@@@";
    GoToPosition(2, 6);
    cout <<
        "@@@@@@;.!@- ,@#.. ,@@@@@@!@* -@@.@@.    *@@@@@@@@@@@@@@#  ......~@@@:......      =@@*    *@@;   .@@,;#,  #@!      ";
    GoToPosition(2, 7);
    cout <<
        "*=@@=**@@@@@@@@@@  --#@:-:@*  ~. @@.   ,@@*!!!=@#!!!!!!;       ,@@*@@~        .;#@#:--~;$@@:    !@!.@@~ !@#. !@:  "
        << endl;
    GoToPosition(2, 8);
    cout <<
        " :@#, :@@      @@    $@, ~@*     @@.   $@=    ~@$             -@@= :@@:       #@@@@@@@@@@=,    ,@@##@= ~@#.  ~@#. ";
    GoToPosition(2, 9);
    cout <<
        " #@@@,:@@@@@@@@@@    $@, ~@*    .@@   *@@.    ~@$           .$@@@#. :@@#,     ;@@@$=@@@@-;.     @@@@@,-@@;--~:#@# ";
    GoToPosition(2, 10);
    cout <<
        ",@@@@$;@@@@@@@@@@    $@$@*@*    ,@@   *@~     ~@$         ~=@@@;*@#: ~#@@$;,   .  :#@@;.~@#-    ,,$@;.@@@@@@@@@@@:"
        << endl;
    GoToPosition(2, 11);
    cout <<
        "!@@#*@#@@      @@ .;#@@@$;@*    -@#   .:*=====$@@=====*  ;@@@=,  !@*   !#@@!   .~#@#*. ..=@@:    :@$  =$#@#*@@;!@#"
        << endl;
    GoToPosition(2, 12);
    cout <<
        "@@## ~:@@@@@@@@@@ ,@@@@- ~@* =: ;@*     @@@@@@@@@@@@@@@. .@!     =;      :=   #@@@@@@@@@@@@@@!  ,@@~~*, ;@; #@, $,";
    GoToPosition(2, 13);
    cout <<
        "@!$#  ~@!:!@=::## .;-$@, ~@==@= *@~     ------;@$------   .;!.#$,@@!   ~#:    $@@@@@@@@##$==@@; $@@@@@- !@: #@,   ";
    GoToPosition(2, 14);
    cout <<
        "# $#      *@=        $@, ~@@@$. @@,           ~@$          #@ ## ~@@;.:;@@-   -;:=-..@@. :. ;#, *#$=!:  =@, #@,   ";
    GoToPosition(2, 15);
    cout <<
        ". $# =@@@@@@@@@@@@.  $@, :@@#  $@@@.          ~@$         -@= ##  -@--@=;@@.    $@@  @@.!@@-         :- #@  #@, !@";
    GoToPosition(2, 16);
    cout <<
        "  $# ,---=@@@@~---   $@, #@=. ;@@$@=          ~@$         #@: ##   . ~@! $@=  ,#@#,  @@..*@@*   :!=#@@!!@=  #@, =@";
    GoToPosition(2, 17);
    cout <<
        "  $#   ,*@@-=@#~.    $@, ~=  ;@@:,@@! ;!!!!!!!=@#*!!!!!!:-@#  #@,,,,,=@~  =~ ~@@$.  .@@.  ;@@$,-@@@#=!*@@-  #@- #@";
    GoToPosition(2, 18);
    cout <<
        "  $#.!@@@#.  =@@@#,@@@@,   .#@@:  ,@@~$@@@@@@@@@@@@@@@@@=.*;  =@@@@@@@@,    :@@= ,@@@@@    .@@# =:  -#@@:   =@@@@@";
    GoToPosition(2, 19);
    cout <<
        "  $#.@@#:.    -=@=.@@@=    -@@~    !$,-~~~~~~~~~~~~~~~~~,     .!=$$$=*-      =:  ,@@@@;     ,*.     ;@#~    ,#@@@;";
    GoToPosition(33, 22);
    cout << "兰陈昕";
    GoToPosition(34, 24);
    cout << "14班";
    GoToPosition(70, 23);
    cout << "请按回车键继续";
    Pause();
}

void ExamplePage()
{
    SetConsoleTitle(TEXT("介绍"));
    PrintBackground();
    GoToPosition(12, 6);
    cout << "本程序一点也不真实";
    GoToPosition(12, 8);
    cout << "基本规则：水草有生长、繁衍、死亡三种行为，鱼有生长、移动、捕食、繁衍、死亡五种行为。";
    GoToPosition(22, 9);
    cout << "水草寿命无限（+1s），只有被吃完后才会死亡；同一水草可以多次繁衍。";
    GoToPosition(22, 10);
    cout << "鱼会死于被捕食或饥饿；鱼进行除捕食外的任何行为都会消耗体力值；一条鱼只能繁衍一次，且无需交配。";
    GoToPosition(18, 12);
    cout << "下面是本程序的图例：";
    GoToPosition(22, 14);
    cout << kIcon[1] << " ：水草";
    GoToPosition(22, 16);
    cout << kIcon[2] << "：小鱼";
    GoToPosition(22, 18);
    cout << kIcon[3] << "：大鱼";
    GoToPosition(20, 20);
    cout << "食物链：" << kIcon[1] << " ->" << kIcon[2] << "->" << kIcon[3];
    GoToPosition(70, 23);
    cout << "请按回车键继续";
    Pause();
}

void SettingPage()
{
    SetConsoleTitle(TEXT("基本参数"));
    PrintBackground();
    GoToPosition(12, 4);
    cout << "下面是本程序的部分基本参数，可在源代码中修改（小心生态系统崩溃）";
    GoToPosition(20, 6);
    cout << "水草生长周期（默认28）：" << kAquaticDevelopTime;
    GoToPosition(20, 8);
    cout << "水草繁衍周期（默认56）：" << kAquaticDeliverTime;
    GoToPosition(20, 10);
    cout << "水草出生率（默认0.4）：" << kAquaticBirthRate;
    GoToPosition(20, 12);
    cout << "小鱼寿命（默认24）：" << kSmallFishLifetime;
    GoToPosition(20, 14);
    cout << "小鱼生育年龄（默认5）：" << kSmallFishDeliverTime;
    GoToPosition(20, 16);
    cout << "小鱼出生率（默认0.6）：" << kSmallFishBirthRate;
    GoToPosition(20, 18);
    cout << "大鱼寿命（默认42）：" << kBigFishLifetime;
    GoToPosition(20, 20);
    cout << "大鱼生育年龄（默认18）：" << kBigFishDeliverTime;
    GoToPosition(20, 22);
    cout << "大鱼出生率（默认0.2）：" << kBigFishBirthRate;
    GoToPosition(60, 6);
    cout << "最大周期（默认320）：" << kDuration;
    GoToPosition(60, 8);
    cout << "演示速度（相对值,默认200）：" << kSleepTime;
    GoToPosition(60, 10);
    cout << "初始最少大鱼数量（默认3）：" << kMinFish;
    GoToPosition(60, 12);
    cout << "初始最多大鱼数量（默认4）：" << kMaxFish;
    GoToPosition(60, 14);
    cout << "初始水草最小长度（默认2）：" << kMinAquatic;
    GoToPosition(60, 16);
    cout << "初始水草最大长度（默认4）：" << kMaxAquatic;
    GoToPosition(70, 23);
    cout << "请按回车键继续";
    Pause();
}

void EndingPage()
{
    SetConsoleTitle(TEXT("结束"));
    PrintBackground();
    GoToPosition(40, 10);
    cout << "演示结束";
    GoToPosition(40, 12);
    cout << information.str;
    GoToPosition(40, 14);
    cout << "本次运行时长：";
    GoToPosition(54, 14);
    pond.ShowTime();
    GoToPosition(70, 23);
    cout << "请按回车键继续";
    Pause();
}

void SetBackground()
{
    for (auto i = 0; i < kWidth >> 1; i++) write[0].str += kIcon[4];
    for (auto i = 1; i < kHeight - 1; i++)
    {
        write[i].str += kIcon[4];
        for (auto j = 2; j < kWidth >> 1; j++) write[i].str += kIcon[0];
        write[i].str += kIcon[4];
    }
    for (auto i = 0; i < kWidth >> 1; i++) write[kHeight - 1].str += kIcon[4];
}


void SetConsole()
{
    //设置背景颜色、文字颜色
    system("COLOR 3F");
    //隐藏指针
    CONSOLE_CURSOR_INFO hidden;
    GetConsoleCursorInfo(kHOut, &hidden);
    hidden.bVisible = 0;
    SetConsoleCursorInfo(kHOut, &hidden);
    //设置窗口大小
    const COORD pos = {kWidth + 1, kHeight + 1};
    SMALL_RECT rc = {0, 0, kWidth, kHeight};
    SetConsoleWindowInfo(kHOut, true, &rc);
    SetConsoleScreenBufferSize(kHOut, pos);
}

void Pause()
{
    char i;
    do { i = static_cast<char>(cin.get()); }
    while (i != '\x0a');
}

inline void PrintBackground()
{
    system("CLS");
    for (const auto & i : write) cout << i.str << endl;
}
