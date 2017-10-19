LEACH Protocol simulator, maybe later add new ones like APTEEN and PEGASUS

Requirements:

1. Sink has many layers which are created by user when adding new sensors and clusters. 
2 The application simulates each type sensor network with multiple layers and hierarchical design. 
3. Before adding any sensors user has to create new sink.
4. By default it creates a new layer when adding first sensor which needs to be a cluster  because sensor can;t directly send data to 
sink. 
5.Each time the user can decide if he wants to add sensor to desired layer by combobox and drags it. 
6. When changed layer view the display also changeds because at the same time only one network layer can be shown. 
7. Add automatic connecting of nodes by it finding the nearest cluster and if it doeasnt have a cluster in its comm range then don;t allow to create this node.
8. Each node has a x,y position on 2d sensor map. 
9. Map and window have static size, which can;t be modified only minimalized.
the map can be scrolled vertically and horizontally so that it can contain more sensors and display them better.
10. Data frames will be divided by time slots according to TDMA

--------------------------------------------------------------------------------------------------------------
Details :

For network objects factory design pattern, even though 
will define only one network type - LEACH. 
