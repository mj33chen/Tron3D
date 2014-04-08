#include "Player.hh"
#include <vector>
#include <queue>
#include <limits>

using namespace std;


#define PLAYER_NAME                 Im_feelin_18
#define PLAYER_NAME_STRING          "Im_feelin_18"

#define PLAYER_SAFE_RADIUS          7
#define BONUS_SAFE_NUM_VERTEX       2

/**
 * Player struct ! 
 */
struct PLAYER_NAME : public Player { 

    /**
     * Player instance pointer. Useful to find the player
     * instance from the "outside".
     */
    static PLAYER_NAME* inst;

    /**
     * Represents a table of distance for reach the vertex (id vertex) of the map 3D.
     * It will be updated in every radar search.
     */
    typedef vector<int> CostTable;

    /**
     *
     *
     */
    typedef vector<bool> Marks;

    /**
     *
     *
     *
     */
    typedef vector<int> Back_to_Dec;

    /**
     *
     *
     *
     */
    //typedef vector<bool> Nw_visited;

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
     * The bike of the player
     */
    Bike NWme;

    /**
     * Costs for every vertex in the last radar of the map
     */
    CostTable table_costs;

    /**
     * Represents a queue of the pending vertexs to visit.
     * It will be updated in every radar
     */
    queue<Vertex> pending;

    /**
     *
     *
     */
    Back_to_Dec back;

    /**
     *
     *
     */
    Marks mark;

     /**
     *
     *
     *
     */
    bool not_obstacle(int id)
    {
        return (not mark[id] and vertex(id).wall == -1 and vertex(id).bike == -1);
    }

    /**
     *
     *
     *
     *
     */
    bool block_NWme(int &root)
    {

        /*vector<int> neighbours3 = vertex(root).neighbours;

        for(int i = 0; i < int(neighbours3.size()); ++i)
        {
            int id3 = neighbours3[i];
            if(vertex(id3).wall == -1) return false;
        }
        return true;*/

        int unbloked = 0;

        queue<Vertex> pending_;
        pending_.push(vertex(root));

        Marks mark_(nb_vertices(), false);

        //CostTable table_costs_(nb_vertices(), numeric_limits<int>::max());

        mark_[root] = true;

        //table_costs_[root] = 0;

        while(not pending_.empty())
        {
            Vertex u = pending_.front();
            vector<int> neighbours3 = vertex(u.id).neighbours;

            pending_.pop();

            for(int i = 0; i < int(neighbours3.size()); ++i)
            {
                int id3 = neighbours3[i];
                if(not mark_[id3] and vertex(id3).wall == -1 and vertex(id3).bike == -1) 
                {
                    mark_[id3] = true;
                    //int new_cost_ = table_costs_[u.id] + 1;
                    
                    //if(new_cost_ < table_costs_[id3])
                    //{
                        //table_costs_[id3] = new_cost_;
                    pending_.push(vertex(id3));
                    ++unbloked;
                    if(unbloked >= PLAYER_SAFE_RADIUS) return false;

                        //cerr << "unbloked = " << unbloked << endl;
                    //}
                }
            }
        }
        return true;
    }

    /**
     *
     *
     *
     */
    /*bool be_found(Bonus bonus)
    {

    }*/

    /**
     *
     *
     *
     */
    bool be_safe(const Vertex &v)
    {
        if(v.bonus == Ghost) return true;

        int num_vertex = 0;
        vector<int> neighbours4 = vertex(v.id).neighbours;
        for(int i = 0; i < int(neighbours4.size()); ++i)
        {
            int id4 = neighbours4[i];
            if((vertex(id4).wall == -1 and vertex(id4).bike == -1) or (vertex(id4).bike == NWme.id)) ++num_vertex;
        }

        if(num_vertex > 1) return true;

        return false;
    }

    /**
     *
     *
     *
     */
    bool non_path(int id)
    {
        vector<int> neighbours3 = vertex(id).neighbours;
        for(int i = 0; i < int(neighbours3.size()); ++i)
        {
            int id3 = neighbours3[i];
            if(vertex(id3).wall == -1 and vertex(id3).bike == -1) return false; 
        }
        return true;
    }

    /**
     *
     *
     */
    bool is_available(const Vertex &v)
    {
        vector<int> neighbours2 = vertex(v.id).neighbours;
        for(int i = 0; i < int(neighbours2.size()); ++i)
        {
            int id = neighbours2[i];
            if(not mark[id])   // the bike NWme position has marked true !!! 
            {
                if(vertex(id).bike != -1) return false;
            }
        }
        return true;
    }

    /**
     * Select the vertexs which is not wall and not has bikes presents
     * @param v         vector of the vertex that the NWme presents
     * @param w         Empty vector
     * @return          Empty vector if has none vertex, else a vector contains 
     *                      vertex available 
     */
    void transformer(const vector<int> &neighbours, vector<int> &empty_neighbours, Movement &movement, bool &mine)
    {
        bool ghost = false;

        vector<int> bikes_neigh;
        vector<int> non_bikes_neigh;
        
        for(int i = 0; i < int(neighbours.size()); ++i)
        {
            int id = neighbours[i];
            if(not_obstacle(id))
            {
                if(is_available(vertex(id)))
                {
                    if(vertex(id).bonus == Ghost) ghost = true;
                    non_bikes_neigh.push_back(id);
                }
                else bikes_neigh.push_back(id);
            }
        }
 
        if(non_bikes_neigh.empty()) empty_neighbours = bikes_neigh;

        else if(non_bikes_neigh.size() == 1)
        {
            if(block_NWme(non_bikes_neigh[0]))
            {
                if(NWme.bonus == Turbo and bikes_neigh.size() == 1) movement.use_bonus = true;

                if(not bikes_neigh.empty()) empty_neighbours = bikes_neigh;

                else empty_neighbours = non_bikes_neigh;
            }

            else empty_neighbours = non_bikes_neigh;
        }

        else if(bikes_neigh.size() >= 1 and ghost and mine)
        {   
            empty_neighbours = bikes_neigh;
        }

        else 
        {
            empty_neighbours = non_bikes_neigh;
        }
    }

    /**
     * Detects bikes inside a radius r from the vertex v.
     * @param v     Position of the vertex as the center to start the radar-
     * @param r     Radius to search.
     * @param bikes Empty vector to store bikes or walls found.
     * @return      True if one or more bike or walls has been found,
     *              false otherwise.
     */
    void radar(const Vertex &v, Movement &movement)
    {

        bool detected = false;

        pending = queue<Vertex>();
        mark = Marks(nb_vertices(), false);

        table_costs = CostTable(nb_vertices(), numeric_limits<int>::max());   // distance from the vertex parent 
        back = Back_to_Dec(nb_vertices(), -2);

        pending.push(v);
        table_costs[v.id] = 0; // distance 0
        back[v.id] = -1;

        int last_position = -1;

        vector<int> empty_neighbours;  // vector of vertexs availables, empty initially 

        while(not pending.empty() and not detected)
        {
            //cerr << "entrado" << endl; 

            Vertex u = pending.front();

            //cerr << "identificador de vertice padre " << u.id << endl;
            last_position = u.id; 
            pending.pop();

            if(not mark[u.id])  
            {
                mark[u.id] = true;  // visited 

                bool mine = false;

                if(u.bonus != None and be_safe(u)) 
                {
                    if(u.bonus == Ghost) mine = true;
                    detected = true;  
                }

                if(NWme.bonus == Points)  
                {
                    movement.use_bonus = true; 
                }

                else if(detected and NWme.bonus == Turbo) 
                {
                    movement.use_bonus = true;   
                }

                else if(detected and NWme.bonus == Ghost)
                {
                    if(u.bonus == Turbo) detected = false;
                }

                /*if(be_found(u.bonus)) detected = true;
                else detected = false;*/

                vector<int> neighbours = vertex(u.id).neighbours;

                bool use = false;

                if(NWme.ghost_duration > 0) 
                {
                    use = true;
                    empty_neighbours = neighbours;
                }

                if(not use) transformer(neighbours, empty_neighbours, movement, mine);

                for(int i = 0; i < int(empty_neighbours.size()); ++i)
                {
                    //if(is_available(vertex(empty_neighbours[i]))) 
                    //{
                        int id = empty_neighbours[i];       
                        int new_cost = table_costs[u.id] + 1;    // add distance 1 from the vertex parent 
                        //

                        if(new_cost < table_costs[id])
                        {
                            table_costs[id] = new_cost;
                            pending.push(vertex(id));            // push vertex 
                            back[id] = u.id;
                        }
                    //}
                }  
            }
        }
     
        int vertex_ = v.id;
        int measure = last_position;

        while(back[measure] != -1)
        {
                vertex_ = measure;
                measure = back[measure]; 
        }

        movement.next_vertex = vertex_;
    }

    bool has_turn()
    {
        return(NWme.alive and (round()%2 == 0 or NWme.turbo_duration > 0));
    }


    /**
     * Play method.
     *  
     * This method will be invoked once per each round.
     * You have to read the board and place your actions
     * for this round.
     */ 
    void play ()
    {

        vector<int> my_bikes = bikes(me());

        if(round() == 0)
        {
            inst = this;
        }

        for(int i = 0; i < (int)my_bikes.size(); ++i)
        {
            NWme = bike(my_bikes[i]);

            // Create an empty movement
            Movement movement(NWme.id);

            if(has_turn())
            {
                bool Edo_Tensei = false;  // NarutoStyle plz ! 

                if(NWme.bonus == Ghost) 
                {
                    if(non_path(NWme.vertex)) 
                    {
                        movement.use_bonus = true;
                    }
                }

                if(NWme.ghost_duration > 0 or movement.use_bonus)
                { 
                    resuscitate_NWme(movement);
                    Edo_Tensei = true;
                }

                if(not Edo_Tensei) radar(vertex(NWme.vertex), movement);

                //pause();

                command(movement);
            }
        }
    }


    /**
     *
     *
     *
     */
    void resuscitate_NWme(Movement &movement)
    {
        bool found = false;

        Marks mark_edo_tensei;
        mark_edo_tensei = Marks(nb_vertices(), false);  // welcome to hell, evil ! 
        mark_edo_tensei[NWme.vertex] = true;

        Back_to_Dec back_ghost;
        back_ghost = Back_to_Dec(nb_vertices(), -2);
        back_ghost[NWme.vertex] = -1;

        CostTable dis;
        dis = CostTable(nb_vertices(), numeric_limits<int>::max());
        dis[NWme.vertex] = 0;

        queue<Vertex> edo_tensei;
        edo_tensei.push(vertex(NWme.vertex));


        int last_ = -1;

        while(not edo_tensei.empty() and not found)
        {
            Vertex u = edo_tensei.front();
            edo_tensei.pop();

            last_ = u.id;

            if(u.wall == -1 and u.bike == -1) found = true;

            else 
            {
                vector<int> neighbours5 = vertex(u.id).neighbours;

                for(int i = 0; i < int(neighbours5.size()); ++i)
                {
                    int id5 = neighbours5[i];
                    if(not mark_edo_tensei[id5]) 
                    {
                        int costs = dis[u.id] + 1;
                        if(costs < dis[id5])
                        {
                            dis[id5] = costs;
                            mark_edo_tensei[id5] = true;
                            edo_tensei.push(vertex(id5));
                            back_ghost[id5] = u.id;
                        }
                    }
                }
            }
        }

        int v_edo_tensei = NWme.vertex;
        int pos = last_;

        while(back_ghost[pos] != -1)
        {
            v_edo_tensei = pos;
            pos = back_ghost[pos];
        }

        movement.next_vertex = v_edo_tensei;
    }

};


PLAYER_NAME* PLAYER_NAME::inst = NULL;

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

