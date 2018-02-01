import numpy as np
import learning as lrn
import model as mdl

def python_function_update(dataset):
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
    # path_n = training_coordinates[training_coordinates[:,1] == 0][:, 0:1]
    # path_p = training_coordinates[training_coordinates[:,1] == 1][:, 0:1]
    # training_coordinates = None  # free memory?
    # parameters
    #### testovani zmeny "sily" periody pri zmene poctu shluku
    longest = 60*60*24*4*7 # testing one day
    shortest = 60*60*2 # testing one day
    #### konec testovani
    edges_of_cell = [60]
    k = 6  # muzeme zkusit i 9
    # hours_of_measurement = 24 * 7  # nepotrebne
    radius = 1
    number_of_periods = 4
    evaluation = True
    C_p, COV_p, density_integrals_p, structure_p, average_p =\
        lrn.proposed_method(longest, shortest, dataset,
                            edges_of_cell, k,
                            radius, number_of_periods, evaluation)
    return C_p, COV_p, density_integrals_p, structure_p, k


def python_function_estimate(whole_model, time):
    """
    input: whole_model tuple of model parameters, specificaly:
                C_p, COV_p, densities_p, structure_p, k_p
           time float, time for prediction
    output: estimation float, estimation of the event occurence
    uses:
    objective: to estimate event occurences in the given time
    """
    ###################################################
    # otevirani a zavirani dveri, pozitivni i negativni
    ###################################################
    freq_p = mdl.one_freq(np.array([[time]]), whole_model[0], whole_model[1],
                             whole_model[3], whole_model[4],
                             whole_model[2])
    return float(freq_p[0])


def python_function_save(whole_model, file_path):
    """
    """
    with open(file_path, 'wb') as opened_file:
        np.savez(opened_file, whole_model[0], whole_model[1], whole_model[2],
                 whole_model[3], whole_model[4])


def python_function_load(file_path):
    """
    """
    with open(file_path, 'r') as opened_file:
        npzfile = np.load(file_path)
        C_p, COV_p, density_integrals_p, structure_p, k =\
            npzfile['arr0'], npzfile['arr1'], npzfile['arr2'],\
            list(npzfile['arr3']), int(npzfile['arr4'])
    return C_p, COV_p, density_integrals_p, structure_p, k
