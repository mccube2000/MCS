from diagrams import Cluster, Diagram
from diagrams.gcp.devtools import SDK as Cache
from diagrams.gcp.devtools import SDK as Device
from diagrams.gcp.devtools import SDK as Executor
from diagrams.gcp.devtools import SDK as Rescurce
from diagrams.gcp.ml import AIHub as Project

with Diagram('Project', show=False, direction='TB'):
    project = Project('Project')

    rescurce = Rescurce('Rescurce')

    with Cluster('Rescurce'):
        printer = Device('Printer')
        cursor = Device('Cursor')
        keyboard = Device('Keyboard')
        screen = Device('Screen')
        storage = Device('Storage')
        cache = Cache('Cache')
        executor = Executor('Executor')

    project - rescurce - [executor, cache, storage, screen, keyboard, cursor, printer]
