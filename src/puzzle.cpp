#include "puzzle.h"
#include "random.h"
#include <iterator>

using Move = CellPosition;

static const Move neighbor_moves[] = {
    {-1, 0}, // top
    {0, -1}, // left
    {1, 0},  // bottom
    {0, 1}   // right
};

Puzzle::Puzzle(size_t size, std::vector<CellTarget> targets) : m_size(size), m_cell_state(m_size), m_num_of_cells_visible(m_size),
                                                                m_articulation_points(m_size), m_can_change_state(m_size), m_targets(targets)
{
    init();
}

void Puzzle::restart()
{
    init();
}

void Puzzle::init()
{
    m_puzzle_solved = false;

    m_cell_state.fill(CellState::in_bag);
    m_num_of_cells_visible.fill(2 * m_size - 1);
    m_articulation_points.fill(false);

    update_can_change_state();
}

void Puzzle::update()
{
    update_articulation_points();
    update_can_change_state();
    check_if_solved();
}

void Puzzle::update_can_change_state()
{
    for (CellIndexType i = 0; i < m_size; i++)
    {
        for (CellIndexType j = 0; j < m_size; j++)
        {
            if (m_cell_state.at(i, j) == CellState::in_bag)
            {
                m_can_change_state.at(i, j) = is_on_bag_border({i, j}) && !m_articulation_points.at(i, j);
            }
            else
            {
                m_can_change_state.at(i, j) = is_outside_bag_border({i, j}) && !m_articulation_points.at(i, j);
            }
        }
    }

    for (auto &[pos, target] : m_targets)
    {
        m_can_change_state[pos] = false;
    }
}

void Puzzle::update_articulation_points()
{
    m_articulation_points.fill(false);
    update_articulation_points_in_bag(get_top_left_pos_in_bag());
    CellPosition top_left = get_top_left_pos_outside_bag();
    if (top_left.i != -1)
    {
        update_articulation_points_outside_bag(top_left);
    }
}

void Puzzle::update_articulation_points_in_bag(CellPosition root)
{
    Cells<size_t> discovery_time(m_cell_state.m_size * m_cell_state.m_size);
    size_t count = 0;

    // check if root has more than one neighbor then it is an articulation point

    m_cell_state[root] = CellState::visited;
    discovery_time[root] = count;
    size_t root_neighbor_count = 0;

    for (auto &move : neighbor_moves)
    {
        CellPosition neighbor = root + move;
        if (m_cell_state.is_legal_position(neighbor) && m_cell_state[neighbor] == CellState::in_bag)
        {
            root_neighbor_count++;
            recursive_dfs_in_bag(neighbor, root, ++count, discovery_time);
        }
    }

    if (root_neighbor_count != 1)
    {
        m_articulation_points[root] = true;
    }

    for (CellIndexType i = 0; i < m_cell_state.m_size; i++)
    {
        for (CellIndexType j = 0; j < m_cell_state.m_size; j++)
        {
            if (m_cell_state.at(i, j) == CellState::visited)
            {
                m_cell_state.at(i, j) = CellState::in_bag;
            }
        }
    }
}

size_t Puzzle::recursive_dfs_in_bag(CellPosition node, CellPosition parent_node, size_t &count, Cells<size_t> &discovery_time)
{
    m_cell_state[node] = CellState::visited;
    discovery_time[node] = count;
    size_t low = count;

    for (auto &move : neighbor_moves)
    {
        CellPosition neighbor = node + move;
        if (m_cell_state.is_legal_position(neighbor))
        {
            if (neighbor != parent_node)
            {
                if (m_cell_state[neighbor] == CellState::in_bag)
                {
                    size_t min_back_edge = recursive_dfs_in_bag(neighbor, node, ++count, discovery_time);
                    if (min_back_edge >= discovery_time[node])
                    {
                        m_articulation_points[node] = true;
                    }
                    low = std::min(low, min_back_edge);
                }
                else if (m_cell_state[neighbor] == CellState::visited)
                {
                    low = std::min(low, discovery_time[neighbor]);
                }
            }
        }
    }

    return low;
}

void Puzzle::update_articulation_points_outside_bag(CellPosition root)
{
    Cells<size_t> discovery_time(m_size * m_size);
    size_t count = 0;

    // top
    for (CellIndexType j = 0; j < m_size; j++)
    {
        if (m_cell_state.at(0, j) == CellState::out_of_bag)
        {
            recursive_dfs_outside_bag({0, j}, {root.i - 1, root.j}, ++count, discovery_time);
        }
    }
    // bottom
    for (CellIndexType j = 0; j < m_size; j++)
    {
        if (m_cell_state.at(m_size - 1, j) == CellState::out_of_bag)
        {
            recursive_dfs_outside_bag({static_cast<CellIndexType>(m_size) - 1, j}, {root.i + 1, root.j}, ++count, discovery_time);
        }
    }
    // left
    for (CellIndexType i = 1; i < m_size - 1; i++)
    {
        if (m_cell_state.at(i, 0) == CellState::out_of_bag)
        {
            recursive_dfs_outside_bag({i, 0}, {root.i, root.j - 1}, ++count, discovery_time);
        }
    }
    // right
    for (CellIndexType i = 1; i < m_size - 1; i++)
    {
        if (m_cell_state.at(i, m_size - 1) == CellState::out_of_bag)
        {
            recursive_dfs_outside_bag({i, static_cast<CellIndexType>(m_size) - 1}, {root.i, root.j + 1}, ++count, discovery_time);
        }
    }

    for (CellIndexType i = 0; i < m_size; i++)
    {
        for (CellIndexType j = 0; j < m_size; j++)
        {
            if (m_cell_state.at(i, j) == CellState::visited)
            {
                m_cell_state.at(i, j) = CellState::out_of_bag;
            }
        }
    }
}

size_t Puzzle::recursive_dfs_outside_bag(CellPosition node, CellPosition parent_node, size_t &count, Cells<size_t> &discovery_time)
{
    m_cell_state[node] = CellState::visited;
    discovery_time[node] = count;
    size_t low = count;

    for (auto &move : neighbor_moves)
    {
        CellPosition neighbor = node + move;
        ;
        if (neighbor != parent_node)
        {
            if (m_cell_state.is_legal_position(neighbor))
            {
                if (m_cell_state[neighbor] == CellState::out_of_bag)
                {
                    size_t min_back_edge = recursive_dfs_outside_bag(neighbor, node, ++count, discovery_time);
                    if (min_back_edge >= discovery_time[node])
                    {
                        m_articulation_points[node] = true;
                    }
                    low = std::min(low, min_back_edge);
                }
                else if (m_cell_state[neighbor] == CellState::visited)
                {
                    low = std::min(low, discovery_time[neighbor]);
                }
            }
            else
            {
                low = std::min(low, (size_t)0);
            }
        }
    }

    return low;
}

CellPosition Puzzle::get_top_left_pos_in_bag()
{
    for (CellIndexType i = 0; i < m_cell_state.m_size; i++)
    {
        for (CellIndexType j = 0; j < m_cell_state.m_size; j++)
        {
            if (m_cell_state.at(i, j) == CellState::in_bag)
            {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

CellPosition Puzzle::get_top_left_pos_outside_bag()
{
    for (CellIndexType i = 0; i < m_size; i++)
    {
        for (CellIndexType j = 0; j < m_size; j++)
        {
            if (m_cell_state.at(i, j) == CellState::out_of_bag)
            {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

bool Puzzle::is_on_bag_border(CellPosition pos)
{
    if (m_cell_state[pos] == CellState::in_bag)
    {
        // top
        if (pos.i == 0 || m_cell_state.at(pos.i - 1, pos.j) == CellState::out_of_bag)
        {
            return true;
        }
        // bottom
        if ((pos.i + 1) >= m_size || m_cell_state.at(pos.i + 1, pos.j) == CellState::out_of_bag)
        {
            return true;
        }
        // left
        if (pos.j == 0 || m_cell_state.at(pos.i, pos.j - 1) == CellState::out_of_bag)
        {
            return true;
        }
        // right
        if ((pos.j + 1) >= m_size || m_cell_state.at(pos.i, pos.j + 1) == CellState::out_of_bag)
        {
            return true;
        }
    }
    return false;
}

bool Puzzle::is_outside_bag_border(CellPosition pos)
{
    if (m_cell_state[pos] == CellState::out_of_bag)
    {
        // top
        if (pos.i != 0 && m_cell_state.at(pos.i - 1, pos.j) == CellState::in_bag)
        {
            return true;
        }
        // bottom
        if ((pos.i + 1) < m_size && m_cell_state.at(pos.i + 1, pos.j) == CellState::in_bag)
        {
            return true;
        }
        // left
        if (pos.j != 0 && m_cell_state.at(pos.i, pos.j - 1) == CellState::in_bag)
        {
            return true;
        }
        // right
        if ((pos.j + 1) < m_size && m_cell_state.at(pos.i, pos.j + 1) == CellState::in_bag)
        {
            return true;
        }
    }
    return false;
}

void Puzzle::trace_bag_border_points(std::vector<CellPosition>& bag_border_points)
{
    enum Facing
    {
        Facing_RIGHT = 0,
        Facing_DOWN = 1,
        Facing_LEFT = 2,
        Facing_UP = 3,
    };

    static const Move go_left_moves[] = {
        [Facing::Facing_RIGHT] = {-1, 1},
        [Facing::Facing_DOWN] = {1, 1},
        [Facing::Facing_LEFT] = {1, -1},
        [Facing::Facing_UP] = {-1, -1}};

    static const Move go_forward_moves[] = {
        [Facing::Facing_RIGHT] = {0, 1},
        [Facing::Facing_DOWN] = {1, 0},
        [Facing::Facing_LEFT] = {0, -1},
        [Facing::Facing_UP] = {-1, 0}};

    static const Move go_left_or_right_border_point_offset[] = {
        [Facing::Facing_RIGHT] = {0, 1},
        [Facing::Facing_DOWN] = {1, 1},
        [Facing::Facing_LEFT] = {1, 0},
        [Facing::Facing_UP] = {0, 0},
    };

    static const Facing go_left_dir_transition[] = {
        [Facing::Facing_RIGHT] = Facing::Facing_UP,
        [Facing::Facing_DOWN] = Facing::Facing_RIGHT,
        [Facing::Facing_LEFT] = Facing::Facing_DOWN,
        [Facing::Facing_UP] = Facing::Facing_LEFT,
    };

    static const Facing go_right_dir_transition[] = {
        [Facing::Facing_RIGHT] = Facing::Facing_DOWN,
        [Facing::Facing_DOWN] = Facing::Facing_LEFT,
        [Facing::Facing_LEFT] = Facing::Facing_UP,
        [Facing::Facing_UP] = Facing::Facing_RIGHT,
    };

    CellPosition starting_pos = get_top_left_pos_in_bag();
    Facing starting_dir = Facing::Facing_RIGHT;

    Facing currently_facing = starting_dir;
    CellPosition current_pos = starting_pos;

    bag_border_points.clear();
    bag_border_points.push_back(current_pos);

    Move next_move;
    do
    {
        // go left
        next_move = current_pos + go_left_moves[currently_facing];
        if (m_cell_state.is_legal_position(next_move))
        {
            if (m_cell_state[next_move] == CellState::in_bag)
            {

                bag_border_points.push_back(current_pos + go_left_or_right_border_point_offset[currently_facing]);
                // make the next move
                current_pos = next_move;
                currently_facing = go_left_dir_transition[currently_facing];
                continue;
            }
        }

        // go forward
        next_move = current_pos + go_forward_moves[currently_facing];
        if (m_cell_state.is_legal_position(next_move))
        {
            if (m_cell_state[next_move] == CellState::in_bag)
            {
                // make the next move
                current_pos = next_move;
                continue;
            }
        }

        // go right
        bag_border_points.push_back(current_pos + go_left_or_right_border_point_offset[currently_facing]);
        currently_facing = go_right_dir_transition[currently_facing];

        // break if returned to the start
        if (current_pos == starting_pos && currently_facing == starting_dir)
        {
            break;
        }

    } while (true);
}

std::unique_ptr<Puzzle> Puzzle::generate_puzzle(size_t size)
{
    static uint64_t seed = Random::get_hourly_seed();
    Random rand(seed++);
    auto puzzle = std::make_unique<Puzzle>(size, std::vector<CellTarget>());
    puzzle->m_seed = seed;
    float r = rand.get_random_float_between_a_inclusive_b_inclusive(0, 1);
    size_t num_empty_cells = (size * size) / (2.2 + r);

    Cells<bool> on_edge(size);
    std::vector<CellPosition> edges;
    for (CellIndexType i = 0; i < size; i++)
    {
        for (CellIndexType j = 0; j < size; j++)
        {
            if (puzzle->is_on_bag_border({i, j}))
            {
                on_edge.at(i, j) = true;
                edges.push_back({i, j});
            }
            else
            {
                on_edge.at(i, j) = false;
            }
        }
    }

    for (size_t i = 0; i < num_empty_cells; i++)
    {
        int random_idx = rand.get_random_int_between_a_inclusive_b_inclusive(0, edges.size()-1);
        for (size_t i = 0; i < edges.size(); i++)
        {
            
            if (!puzzle->m_articulation_points[edges[(random_idx + i) % edges.size()]])
            {
                random_idx = (random_idx + i) % edges.size();
                break;
            }
            if (i == edges.size() - 1)
            { // reached end without finding an elemnt
                goto calc_targets_label;
            }
        }
        CellPosition random_edge = edges[random_idx];
        puzzle->remove_from_bag(random_edge);
        std::swap(edges.back(), edges[random_idx]);
        edges.pop_back();
        on_edge[random_edge] = false;

        // check if his neighbors updated into edges
        for (auto &move : neighbor_moves)
        {
            CellPosition neighbor = random_edge + move;
            if (puzzle->m_cell_state.is_legal_position(neighbor))
            {
                if (!on_edge.at(neighbor.i, neighbor.j) && puzzle->is_on_bag_border(neighbor))
                {
                    on_edge.at(neighbor.i, neighbor.j) = true;
                    edges.push_back(neighbor);
                }
            }
        }
    }
//=============================================
// label
calc_targets_label:
    //---------------------------------------------
    // calculate the targets for the choosen cells
    //-------------------------------------------------
    Cells<int32_t> targets(size, -3);
    // top
    for (CellIndexType j = 0; j < size; j++)
    {
        int32_t count = 0;
        for (CellIndexType i = 0; i < size; i++)
        {
            if (puzzle->m_cell_state.at(i, j) == CellState::in_bag)
            {
                count++;
            }
            else
            {
                count = 0;
            }

            targets.at(i, j) += count;
        }
    }
    // left
    for (CellIndexType i = 0; i < size; i++)
    {
        int32_t count = 0;
        for (CellIndexType j = 0; j < size; j++)
        {
            if (puzzle->m_cell_state.at(i, j) == CellState::in_bag)
            {
                count++;
            }
            else
            {
                count = 0;
            }

            targets.at(i, j) += count;
        }
    }
    // right
    for (CellIndexType i = 0; i < size; i++)
    {
        int32_t count = 0;
        for (CellIndexType j = size - 1; j >= 0; j--)
        {
            if (puzzle->m_cell_state.at(i, j) == CellState::in_bag)
            {
                count++;
            }
            else
            {
                count = 0;
            }

            targets.at(i, j) += count;
        }
    }
    // bottom
    for (CellIndexType j = 0; j < size; j++)
    {
        int32_t count = 0;
        for (CellIndexType i = size - 1; i >= 0; i--)
        {
            if (puzzle->m_cell_state.at(i, j) == CellState::in_bag)
            {
                count++;
            }
            else
            {
                count = 0;
            }

            targets.at(i, j) += count;
        }
    }

    puzzle->m_targets.clear();
    // for (CellIndexType i = 0; i < size; i++)
    // {
    //     for (CellIndexType j = 0; j < size; j++)
    //     {
    //         if (puzzle->is_in_bag({i, j}))
    //         {
    //             puzzle->m_targets.push_back({{i, j}, targets.at(i, j)});
    //         }
    //     }
    // }

    std::vector<CellTarget> candidates;
    for (CellIndexType i = 0; i < size; i++)
    {
        candidates.clear();
        for (CellIndexType j = 0; j < size; j++)
        {
            if (puzzle->is_in_bag({i,j}))
            {
                candidates.push_back({{i, j}, targets.at(i, j)});
            }
        }
        if (candidates.size() >= 1)
        {
            int32_t sammple_size = 1;
            if (candidates.size() >= 3)
            {
                int32_t rand_number = rand.get_random_int_between_a_inclusive_b_inclusive(0, 100);
                if (rand_number < 20)
                {
                    sammple_size = 1;
                }
                else if (rand_number < 80)
                {
                    sammple_size = 2;
                }
                else
                {
                    sammple_size = 3;
                }
            }
            std::sample(candidates.begin(), candidates.end(), std::back_inserter(puzzle->m_targets), sammple_size, rand.rng);
        }
    }

    puzzle->restart();

    return puzzle;
}

void Puzzle::check_if_solved()
{
    for (auto &[pos, target] : m_targets)
    {
        if (m_num_of_cells_visible[pos] != target)
        {
            m_puzzle_solved = false;
            return;
        }
    }
    m_puzzle_solved = true;
}

bool Puzzle::can_remove_from_bag(CellPosition pos)
{
    return (m_cell_state[pos] == CellState::in_bag) && m_can_change_state[pos];
}

bool Puzzle::can_put_back_in_bag(CellPosition pos)
{
    return (m_cell_state[pos] == CellState::out_of_bag) && m_can_change_state[pos];
}

void Puzzle::remove_from_bag(CellPosition pos)
{
    // update m_num_of_cells_visible
    CellIndexType iter;
    size_t visable_cells;

    // sum top
    visable_cells = 0;
    for (iter = pos.i; iter >= 0 && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter--)
    {
        ++visable_cells;
    }
    // subtract from bottom
    for (iter = pos.i; iter < m_size && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter++)
    {
        m_num_of_cells_visible.at(iter, pos.j) -= visable_cells;
    }

    // sum bottom
    visable_cells = 0;
    for (iter = pos.i; iter < m_size && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter++)
    {
        ++visable_cells;
    }
    // subtract from top
    for (iter = pos.i; iter >= 0 && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter--)
    {
        m_num_of_cells_visible.at(iter, pos.j) -= visable_cells;
    }

    // sum left
    visable_cells = 0;
    for (iter = pos.j; iter >= 0 && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter--)
    {
        ++visable_cells;
    }
    // subtract from right
    for (iter = pos.j; iter < m_size && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter++)
    {
        m_num_of_cells_visible.at(pos.i, iter) -= visable_cells;
    }

    // sum right
    visable_cells = 0;
    for (iter = pos.j; iter < m_size && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter++)
    {
        ++visable_cells;
    }
    // subtract from left
    for (iter = pos.j; iter >= 0 && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter--)
    {
        m_num_of_cells_visible.at(pos.i, iter) -= visable_cells;
    }

    // take out of the bag
    m_cell_state[pos] = CellState::out_of_bag;
    m_num_of_cells_visible[pos] = 0;

    update();
}

void Puzzle::put_back_in_bag(CellPosition pos)
{
      // put back in the bag
      m_cell_state[pos] = CellState::in_bag;

      // update m_num_of_cells_visible
      {
          int32_t iter;
          size_t visable_cells;
  
          // sum top
          visable_cells = 0;
          for (iter = pos.i; iter >= 0 && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter--)
          {
              ++visable_cells;
          }
          // subtract from bottom
          for (iter = pos.i; iter < m_size && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter++)
          {
              // std::cout << "bottom: cell " << iter << "," << j << " -> -" << visable_cells << std::endl;
              m_num_of_cells_visible.at(iter, pos.j) += visable_cells;
          }
  
          // sum bottom
          visable_cells = 0;
          for (iter = pos.i; iter < m_size && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter++)
          {
              ++visable_cells;
          }
          // subtract from top
          for (iter = pos.i; iter >= 0 && m_cell_state.at(iter, pos.j) == CellState::in_bag; iter--)
          {
              // std::cout << "top: cell " << iter << "," << j << " -> -" << visable_cells << std::endl;
              m_num_of_cells_visible.at(iter, pos.j) += visable_cells;
          }
  
          // sum left
          visable_cells = 0;
          for (iter = pos.j; iter >= 0 && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter--)
          {
              ++visable_cells;
          }
          // subtract from right
          for (iter = pos.j; iter < m_size && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter++)
          {
              // std::cout << "right: cell " << i << "," << iter << " -> -" << visable_cells << std::endl;
              m_num_of_cells_visible.at(pos.i, iter) += visable_cells;
          }
  
          // sum right
          visable_cells = 0;
          for (iter = pos.j; iter < m_size && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter++)
          {
              ++visable_cells;
          }
          // subtract from left
          for (iter = pos.j; iter >= 0 && m_cell_state.at(pos.i, iter) == CellState::in_bag; iter--)
          {
              // std::cout << "left: cell " << i << "," << iter << " -> -" << visable_cells << std::endl;
              m_num_of_cells_visible.at(pos.i, iter) += visable_cells;
          }
  
          // correct multiple adds
          m_num_of_cells_visible[pos] -= 3;
      }
      update();
}

bool Puzzle::is_solved()
{
    return m_puzzle_solved;
}

bool Puzzle::is_in_bag(CellPosition pos)
{
    return m_cell_state[pos] == CellState::in_bag;
}

int32_t Puzzle::get_num_cells_visible_from(CellPosition pos)
{
    return m_num_of_cells_visible[pos];
}

const std::vector<CellTarget> &Puzzle::get_targets()
{
    return m_targets;
}

size_t Puzzle::get_size()
{
    return m_size;
}
