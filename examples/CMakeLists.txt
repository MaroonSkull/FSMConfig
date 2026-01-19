# ============================================================================
# Simple FSM Example
# ============================================================================
add_executable(simple_fsm simple_fsm/main.cpp)
target_link_libraries(simple_fsm PRIVATE fsmconfig)

# ============================================================================
# Game State Example
# ============================================================================
add_executable(game_state game_state/main.cpp)
target_link_libraries(game_state PRIVATE fsmconfig)

# ============================================================================
# Network Protocol Example
# ============================================================================
add_executable(network_protocol network_protocol/main.cpp)
target_link_libraries(network_protocol PRIVATE fsmconfig)
