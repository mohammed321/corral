#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

enum class CellState {
    out_of_bag,
    in_bag,
    visited
};

using CellIndexType = int32_t;

struct CellPosition {
    CellIndexType i;
    CellIndexType j;

    CellPosition operator+(const CellPosition& other) {
        return {
            i + other.i,
            j + other.j
        };
    }

    bool operator==(const CellPosition& other) {
        return i == other.i && j == other.j;
    }

    bool operator!=(const CellPosition& other) {
        return !(*this == other);
    }
};

template<typename T>
struct Cells {
    T* m_buffer;
    size_t m_size;

    Cells(size_t size, T default_value) : Cells(size) {
        fill(default_value);
    }

    Cells(size_t size) : m_size(size) {
        m_buffer = new T[m_size * m_size];
    }

    ~Cells() {
        delete[] m_buffer;
    }

    void fill(T value) {
        for (CellIndexType i = 0; i < m_size; i++) {
            for (CellIndexType j = 0; j < m_size; j++) {
                at(i, j) = value;
            }
        }
    }

    bool is_legal_position(const CellPosition& pos) {
        return pos.i >= 0 && pos.j >= 0 && pos.i < m_size && pos.j < m_size;
    }

    T& operator[](const CellPosition& pos) {
        return at(pos.i, pos.j);
    }

    T& at(const CellIndexType& i, const CellIndexType& j) {
        return m_buffer[i*m_size + j];
    }

    
};

struct CellTarget{
    CellPosition pos;
    int32_t target;
};

class Puzzle {
public:
    Puzzle(size_t size, std::vector<CellTarget> targets);

    void restart();

    bool can_remove_from_bag(CellPosition pos);
    bool can_put_back_in_bag(CellPosition pos);

    void remove_from_bag(CellPosition pos);
    void put_back_in_bag(CellPosition pos);

    bool is_solved();
    bool is_in_bag(CellPosition pos);
    int32_t get_num_cells_visible_from(CellPosition pos);
    const std::vector<CellTarget>& get_targets();
    size_t get_size();

    void trace_bag_border_points(std::vector<CellPosition>& bag_border_points);

    static std::unique_ptr<Puzzle> generate_puzzle(size_t size);

    uint64_t m_seed;

private:
    size_t m_size;
    Cells<CellState> m_cell_state;
    Cells<int32_t> m_num_of_cells_visible;
    Cells<bool> m_articulation_points;
    Cells<bool> m_can_change_state;
    std::vector<CellTarget> m_targets;

    bool m_puzzle_solved;

    void init();
    void update();

    void update_can_change_state();

    void update_articulation_points();
    void update_articulation_points_in_bag(CellPosition root);
    size_t recursive_dfs_in_bag(CellPosition node, CellPosition parent_node, size_t &count, Cells<size_t> &discovery_time);
    void update_articulation_points_outside_bag(CellPosition root);
    size_t recursive_dfs_outside_bag(CellPosition node, CellPosition parent_node, size_t& count, Cells<size_t>& discovery_time);

    CellPosition get_top_left_pos_in_bag();
    CellPosition get_top_left_pos_outside_bag();

    bool is_on_bag_border(CellPosition pos);
    bool is_outside_bag_border(CellPosition pos);

    void check_if_solved();
};
