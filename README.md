## Interpreter project in CS2205-aut

We develop this interpreter based on the template of interpreter and frontend code given in class.

##### The rule of the grammar tree: (to be continued) 

E :: = 

       N | V | -E | E + E | E - E | E * E | 

       E / E | E % E | E < E | E <= E | E == E |

       E != E | E >= E | E > E | E && E | E || E | 

       !E | *E | &E | FUNC_NAME(E, E, ..., E) 


C ::  = 
        
        int V; C |

        int ptr V; C |

        int ptr ... ptr V; C |

        (I call them type V below)
        
        type V = E; C |

        type ref V = V; C |

        E = E |

        C; C |

        if (E) then { C } else { C } |

        while (E) do { C } |

        for { C } ( E ) { C } { C } |

        do { C } while (E) |

        PROC_NAME (E, E, ..., E) |

        continue | break | return


GLOB_ITEM :: =  
        
                int V |

                int ptr V |

                int ptr ... ptr V |

                (I call them type V below)

                func type FUNC_NAME (type (ref) V, type (ref) V, ..., type (ref) V) { C } |

                proc PROC_NAME (type (ref) V, type (ref) V, ..., type (ref) V) { C }


P ::= 

        GLOB_ITEM ; GLOB_ITEM ; ... ; GLOB_ITEM