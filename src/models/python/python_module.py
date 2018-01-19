import numpy as np
import learning as lrn
import model as mdl

def python_function_update(training_coordinates):
    """
    input: training_coordinates numpy array nxd, measured values in measured
                                                 times
    output: probably whole model
    uses: 
    objective: to call warpHypertime and return all parameters of the found
               model
    """
    ###################################################
    # otevirani a zavirani dveri, pozitivni i negativni
    ###################################################
    
    # differentiate to positives and negatives
    path_n = training_coordinates[training_coordinates[:,1] == 0][:, 0:1]
    path_p = training_coordinates[training_coordinates[:,1] == 1][:, 0:1]
    training_coordinates = None  # free memory?
    # parameters
    longest = 60*60*24*28
    shortest = 60*60*4
    edge_of_square = 1  # nepotrebna hodnota
    timestep = 1
    k = 9  # muzeme zkusit i 9
    # hours_of_measurement = 24 * 7  # nepotrebne
    radius = 2
    number_of_periods = 2
    evaluation = False

    #positves and negatives binarni
    print('\nmodel for positive measurements\n\n\n')
    C_p, COV_p, density_integrals_p, structure_p, average_p =\
        lrn.proposed_method(longest, shortest, path_p, edge_of_square, timestep, k,
                            radius, number_of_periods, evaluation)
    print('\nmodel for negative measurements\n\n\n')
    C_n, COV_n, density_integrals_n, structure_n, average_n =\
        lrn.proposed_method(longest, shortest, path_n, edge_of_square, timestep, k,
                        radius, number_of_periods, evaluation)
    return C_p, COV_p, density_integrals_p, structure_p, k,\
        C_n, COV_n, density_integrals_n, structure_n  # , k


def python_function_estimate(whole_model, time):
    """
    input: whole_model tuple of model parameters, specificaly:
                C_p, COV_p, densities_p, structure_p, k_p
                a
                C_n, COV_n, densities_n, structure_n, k_n
           time float, time for prediction
    output: estimation float, estimation of the event occurence
    uses:
    objective: to estimate event occurences in the given time
    """
    ###################################################
    # otevirani a zavirani dveri, pozitivni i negativni
    ###################################################
    print('jsem tu a cas je:')
    print(time)
    freq_p = mdl.frequencies(np.array([[time]]), whole_model[0], whole_model[1],
                             whole_model[3], whole_model[4],
                             whole_model[2])
    print(freq_p)
    freq_n = mdl.frequencies(np.array([[time]]), whole_model[5], whole_model[6],
                             whole_model[8], whole_model[4],
                             whole_model[7])
    print(freq_n)
    probability = freq_p / (freq_p + freq_n)
    print(probability)
    return float(probability[0])

    





