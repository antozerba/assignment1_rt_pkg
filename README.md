# Assignment 1 - Research Track

##  Description

A collision-avoidance control system for two TurtleSim turtles with automatic boundary management. The project implements a multi-node ROS2 architecture that enables independent control of two simulated robots with anti-collision safety and automatic bounce-back functionality.


## ️ Prerequisites

- **ROS2** 
- **C++ 17 or higher**
- **TurtleSim package**
- **CMake 3.8+**

##  Installation

1. **Clone the repository into your ROS2 workspace:**

```bash
cd ~/ros2_ws/src
git clone https://github.com/antozerba/assignment1_rt.git
```

2. **Build the workspace:**s

```bash
cd ~/ros2_ws
colcon build --packages-select assignment1_rt
```

3. **Source the setup file:**

```bash
source install/setup.bash
```

##  Usage

### 1. Launch TurtleSim

In the first terminal:

```bash
ros2 run turtlesim turtlesim_node
```

### 2. Spawn the second turtle

In a second terminal:

```bash
ros2 run assignment1_rt turtle_spawn.py
```

### 3. Start the distance calculation node

In a third terminal:

```bash
ros2 run assignment1_rt distance_node
```

### 4. Start the UI control node

In a fourth terminal:

```bash
ros2 run assignment1_rt ui_node
```

### System Interaction

When the UI node is active, you can control the turtles:

```
Choose turtle: 
1                    # Choose turtle 1 or 2
Choose linear vel: 
2.0                  # Linear velocity (m/s)
Choose angular vel: 
0.5                  # Angular velocity (rad/s)
```



### ROS2 Nodes

#### 1. **UI Node** (`ui_node`)
- **Function**: Manages user input and publishes velocity commands
- **Features**:
  - Terminal interface for control
  - Automatic bounce-back system on collision
  - Emergency stop handling

#### 2. **Distance Node** (`distance_node`)
- **Function**: Monitors positions and distances, ensures safety
- **Features**:
  - Continuous distance calculation between turtles
  - Collision detection (threshold: 0.5m)
  - Boundary control (area: 1.0m - 10.0m)
  - Automatic stop command publishing

### ROS2 Topics

| Topic | Type | Description |
|-------|------|-------------|
| `/turtle1/cmd_vel` | `geometry_msgs/msg/Twist` | Velocity commands for turtle 1 |
| `/turtle2/cmd_vel` | `geometry_msgs/msg/Twist` | Velocity commands for turtle 2 |
| `/turtle1/pose` | `turtlesim/msg/Pose` | Position of turtle 1 |
| `/turtle2/pose` | `turtlesim/msg/Pose` | Position of turtle 2 |
| `/distance` | `std_msgs/msg/Float32` | Calculated distance between turtles |
| `/stop_cmd` | `std_msgs/msg/Bool` | Stop signal for collisions/boundaries |



## Configuration Parameters

### Distance Node
- **Collision threshold**: `0.5` meters
- **Minimum boundary**: `1.0` meters
- **Maximum boundary**: `10.0` meters
- **Control frequency**: `100 Hz` (every 10ms)

### UI Node
- **Command duration**: `1.0` second per movement
- **Bounce-back velocity**: `±2.0` m/s (opposite direction)
- **Bounce-back duration**: `1.0` second


## Execution Flow

1. **User input** → UI Node receives turtle choice and velocities
2. **Publishing** → UI Node sends `/cmd_vel` command for 1 second
3. **Monitoring** → Distance Node calculates distance and checks boundaries
4. **Detection** → If distance < 0.5m or out of bounds → publishes `/stop_cmd`
5. **Reaction** → UI Node receives stop, halts movement and activates bounce-back
6. **Bounce-back** → Reverse movement with vel = -2.0 or 2.0 for 1 second
7. **Reset** → System ready for new command

## Safety Behavior

The system implements three safety levels:

1. **Collision prevention**: Automatic stop when distance < 0.5m
2. **Boundary management**: Stop if turtle exits 1.0-10.0m area
3. **Bounce-back**: Automatic reverse movement after collision

## Troubleshooting

### Turtles not moving
- Verify all 4 terminals are active
- Check that the second turtle was spawned correctly
- Check logs for stop messages

### Collisions not detected
- Ensure Distance Node is running
- Verify logs: `ros2 topic echo /distance`

### Bounce-back not working
- Bounce-back only activates after a movement causing collision
- Check logs for "Collision detected!" message

## 🔍 Debug and Monitoring

### View distance in real-time
```bash
ros2 topic echo /distance
```

### Monitor stop commands
```bash
ros2 topic echo /stop_cmd
```

### View turtle velocities
```bash
ros2 topic echo /turtle1/cmd_vel
ros2 topic echo /turtle2/cmd_vel
```


