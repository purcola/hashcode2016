/*
 * hashcode 2016
 *
 * Team: RoPeRT
 * Authors: Luis Riazuelo, Domenico Sicignano, Danilo Tardioli, Pablo Urcola (team leader)
 *
 * Date: 11/03/2016
 * 
 */

#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>


//classes
class Order
{
public:
    int x, y;
    std::vector<int> products; //products[i] = tipo de producto comprado
};

class Warehouse
{

public:
    static std::vector<int> weights; //weights[i] = peso del producto i por unidad

    int x, y;
    std::vector<int> products; //products[i] = nº de productos del tipo i
};

class Drone
{

public:
    static int max_payload;
    int x, y;
    std::vector<int> load; //load[i] = nº de productos del tipo i
};


class Task
{
public:
    int x, y;
    int order;
    int product;
    int quantity;
};

//static variables
std::vector<int> Warehouse::weights;
int Drone::max_payload;

int cost(const Drone& d, const Warehouse& w)
{
    return std::ceil(hypot(d.x - w.x, d.y - w.y));
}

int cost(const Drone& d, const Order& w)
{
    return std::ceil(hypot(d.x - w.x, d.y - w.y));
}

int cost(const Warehouse& d, const Order& w)
{
    return std::ceil(hypot(d.x - w.x, d.y - w.y));
}

void find(const std::vector<Warehouse>& w, const Drone& d, int p, int n, int& bw, int& m)
{
    int best = -1;
    int best_m = 0;
    int min_distance = 1000000;

    for (int i = 0; i < w.size(); ++i)
    {
        if (w[i].products[p] >= n and cost(d, w[i]) < min_distance)
        {
            best = i;
            best_m = w[i].products[p];
            min_distance = cost(d, w[i]);
        }
        else if (w[i].products[p] > best_m)
        {
            best = i;
            best_m = w[i].products[p];
            min_distance = cost(d, w[i]);
        }
        else if (w[i].products[p] == best_m and cost(d, w[i]) < min_distance)
        {
            best = i;
            best_m = w[i].products[p];
            min_distance = cost(d, w[i]);
        }
    }

    bw = best;
    m = std::min(n, best_m);
}

int rows, cols;
int turns;
std::vector<Order> orders;
std::vector<Warehouse> warehouses;
std::vector<Drone> drones;
std::vector<int> weights;

int parse(const std::string& filename)
{
    //  string c = "data" + argv[1];
    std::ifstream infile(filename.c_str());
    //cout << "The name used to start the program: " << argv[ 0 ]
    //     << "\nArguments are:\n";
    //for (int n = 1; n < argc; n++)
    //  cout << setw( 2 ) << n << ": " << argv[ n ] << '\n';

    int r, c, d, t, p;
    int pt;
    int pw;
    int wn;
    int on;
    int wPoseX, wPoseY;
    int oDeliverX, oDeliverY;
    int itNumOrder;
    //fprintf(stderr,"Parse input data\n");

    // map size
    std::string line;
    if (std::getline(infile, line)){
        std::istringstream iss(line);
        if (!(iss >> r >> c >> d >> t >> p)) {return 0; } // error
        fprintf(stderr,"%d %d %d %d %d\n",r,c,d,t,p);
        rows = r;
        cols = c;
        turns = t;
        Drone::max_payload=p;
        drones.resize(d);
    }
    // Product types
    if (std::getline(infile, line)){
        std::istringstream iss(line);
        if (!(iss >> pt)) {return 0; } // error
        fprintf(stderr,"%d\n",pt);
    }
    // Product weigh
    if (std::getline(infile, line)){
        std::istringstream iss(line);
        for (int i=0; i< pt; i++){
            if (!(iss >> pw)) {return 0; } // error
            if (i==pt-1) fprintf(stderr,"%d",pw);
            else fprintf(stderr,"%d ",pw);
            weights.push_back(pw);
        }
    }
    // warehouses number
    if (std::getline(infile, line)){
        std::istringstream iss(line);
        if (!(iss >> wn)) {return 0; } // error
        fprintf(stderr,"\n%d\n",wn);
    }

    // Each warehouse
    for (int i=0; i< wn; i++){
        if (std::getline(infile, line)){
            std::istringstream iss(line);
            if (!(iss >> wPoseX >> wPoseY)) {return 0; } // error
            fprintf(stderr,"%d %d\n",wPoseX, wPoseY);
        }

        Warehouse W;
        W.x=wPoseX;
        W.y=wPoseY;

        int itemsProduct;
        if (std::getline(infile, line)){
            std::istringstream iss(line);
            for (int j=0; j< pt; j++){
                if (!(iss >> itemsProduct)) {return 0; } // error
                if (j==pt-1) fprintf(stderr,"%d", itemsProduct);
                else fprintf(stderr,"%d ", itemsProduct);
                W.products.push_back(itemsProduct);
            }
        }
        fprintf(stderr,"\n");
        warehouses.push_back(W);
    }

    // orders
    if (std::getline(infile, line)){
        std::istringstream iss(line);
        if (!(iss >> on)) {return 0; } // error
        fprintf(stderr,"%d\n",on);
    }

    // Each order
    for (int i=0; i< on; i++){
        if (std::getline(infile, line)){
            std::istringstream iss(line);
            if (!(iss >> oDeliverX >> oDeliverY)) {return 0; } // error
            fprintf(stderr,"%d %d\n",oDeliverX, oDeliverY);
        }
        if (std::getline(infile, line)){
            std::istringstream iss(line);
            if (!(iss >> itNumOrder)) {return 0; } // error
            fprintf(stderr,"%d\n",itNumOrder);
        }
        Order O;
        O.x = oDeliverX;
        O.y = oDeliverY;

        int itemsProductType;
        if (std::getline(infile, line)){
            std::istringstream iss(line);
            for (int j=0; j< itNumOrder; j++){
                if (!(iss >> itemsProductType)) {return 0; } // error
                if (j==itNumOrder-1) fprintf(stderr,"%d", itemsProductType);
                else fprintf(stderr,"%d ", itemsProductType);
                O.products.push_back(itemsProductType);
            }
        }
        fprintf(stderr,"\n");
        orders.push_back(O);
    }
}

int main(int argc, char * argv[])
{
    parse(argv[1]);

    std::vector<Task> tasks;
    for (int ord = 0; ord < orders.size(); ++ord)
    {
        std::sort(orders[ord].products.begin(), orders[ord].products.end());

        for (int i = 0; i < orders[ord].products.size(); ++i)
        {
            Task t;
            t.x = orders[ord].x;
            t.y = orders[ord].y;
            t.quantity = 1;
            t.product = orders[ord].products[i];
            t.order = ord;

            if ((tasks.size() > 0) and (tasks.back().order == ord) and (tasks.back().product == t.product) and ((tasks.back().quantity + 1)*weights[t.product] < Drone::max_payload))
                tasks.back().quantity++;
            else
            {
                tasks.push_back(t);
            }
        }
    }

      std::ostringstream oss;
    //std::cout << 2*tasks.size() << std::endl;
    int n_tasks;

    std::vector<int> steps(drones.size(), 0);

    for (int t = 0; t < tasks.size(); t++)
    {
        int d = t%drones.size();
        int w, n;
        find(warehouses, drones[d], tasks[t].product, tasks[t].quantity, w, n);
        assert(n > 0);
        if (n < tasks[t].quantity)
            std::cerr << "N = " << n << " Q = " << tasks[t].quantity << std::endl;
        assert(n <= tasks[t].quantity);

        int task_cost = cost(drones[d], warehouses[w]) + 1;
        task_cost += cost(warehouses[w], orders[tasks[t].order]) + 1;

        if (steps[d] + task_cost >= turns)
            break;
        steps[d] += task_cost;

        oss << d << ' ' << 'L' << ' ' << w << ' ' <<  tasks[t].product << ' ' << n << std::endl;
        oss << d << ' ' << 'D' << ' ' << tasks[t].order << ' ' <<  tasks[t].product << ' ' << n << std::endl;

        warehouses[w].products[tasks[t].product] -= n;
        drones[d].x = orders[tasks[t].order].x;
        drones[d].y = orders[tasks[t].order].y;
        n_tasks += 2;

        if (n < tasks[t].quantity)
        {
            tasks[t].quantity -= n;
            t--;
        }
    }

    std::cout << n_tasks << std::endl;
    std::cout << oss.str() << std::endl;

    return 0;
}
