from diagrams import Cluster, Diagram
from diagrams.gcp.compute import Run as Executor
from diagrams.gcp.ml import AIHub as Project

with Diagram('Project', show=False, direction='TB'):
    project = Project('Project')

    rescurce = Project('Rescurce')

    with Cluster('Rescurce'):
        executor = Executor('Executor')
        cache = Executor('Cache')
        device = Executor('Device')

    project - rescurce - [executor, cache, device]
