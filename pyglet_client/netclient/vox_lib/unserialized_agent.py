agent =\
{
    'dim': (8,8,8),
    'vosize': 0.2,
    'voxels': [\
        # RED
        [1,1,1, 255, 0, 0, 255],    # row 1
        [1,2,1, 255, 0, 0, 255],
        [1,3,1, 255, 0, 0, 255],
        [1,4,1, 255, 0, 0, 255],
        [1,5,1, 255, 0, 0, 255],
        
        [1,1,2, 255, 0, 0, 255],    # row 2
        [1,3,2, 255, 0, 0, 255],
        [1,5,2, 255, 0, 0, 255],

        [1,1,3, 255, 0, 0, 255],    # row 3
        [1,2,3, 255, 0, 0, 255],
        [1,3,3, 255, 0, 0, 255],
        [1,4,3, 255, 0, 0, 255],
        [1,5,3, 255, 0, 0, 255],
        
        [1,2,2, 0, 255, 0, 255],    # row 2
        [1,4,2, 0, 255, 0, 255],
    ],
}


import json

with open('../media/vox/agent.vox', 'w') as f:
    json.dump(agent, f)
