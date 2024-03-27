import plotly.graph_objects as go
import numpy as np

def rotate_point(point, angle_x, angle_y, angle_z):
    # Convert angles from degrees to radians for numpy
    angle_x, angle_y, angle_z = np.radians([angle_x, angle_y, angle_z])
    
    # Rotation matrices around the x, y, and z axis
    Rx = np.array([[1, 0, 0],
                   [0, np.cos(angle_x), -np.sin(angle_x)],
                   [0, np.sin(angle_x), np.cos(angle_x)]])
    
    Ry = np.array([[np.cos(angle_y), 0, np.sin(angle_y)],
                   [0, 1, 0],
                   [-np.sin(angle_y), 0, np.cos(angle_y)]])
    
    Rz = np.array([[np.cos(angle_z), -np.sin(angle_z), 0],
                   [np.sin(angle_z), np.cos(angle_z), 0],
                   [0, 0, 1]])
    
    # Combined rotation matrix
    R = np.dot(Rz, np.dot(Ry, Rx))
    
    # Apply rotation
    return np.dot(R, point)

def draw_cube(angle_x, angle_y, angle_z):
    # Cube vertices
    vertices = np.array([[0, 0, 0], [.5, 0, 0], [.5, 1, 0], [0, 1, 0],
                         [0, 0, 1], [.5, 0, 1], [.5, 1, 1], [0, 1, 1]])
    
    # Rotate vertices
    rotated_vertices = np.array([rotate_point(v, angle_x, angle_y, angle_z) for v in vertices])
    
    # Cube edges
    edges = [(0, 1), (1, 2), (2, 3), (3, 0), 
             (4, 5), (5, 6), (6, 7), (7, 4), 
             (0, 4), (1, 5), (2, 6), (3, 7)]
    
    # Create the figure
    fig = go.Figure()
    
    for edge in edges:
        fig.add_trace(go.Scatter3d(x=rotated_vertices[[edge[0], edge[1]], 0],
                                   y=rotated_vertices[[edge[0], edge[1]], 1],
                                   z=rotated_vertices[[edge[0], edge[1]], 2],
                                   mode='lines'))
    
    # Update layout for a better view
    fig.update_layout(scene=dict(
                        xaxis=dict(nticks=4, range=[-2.5,2.5]),
                        yaxis=dict(nticks=4, range=[-2.5,2.5]),
                        zaxis=dict(nticks=4, range=[-2.5,2.5])),
                      width=700,
                      margin=dict(r=20, b=10, l=10, t=10))
    
    fig.show()

# Example usage
draw_cube(0,0,0) # Rotate cube 30° around x-axis, 45° around y-axis, 60° around z-axis

