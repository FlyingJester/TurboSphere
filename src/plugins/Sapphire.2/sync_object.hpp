#ifndef SAPPHIRE_SYNC_OBJECT_HEAD
#define SAPPHIRE_SYNC_OBJECT_HEAD

/////
// Sapphire's Synchro object.
//
// A Synchro object is conceptually an object that is created to go along with
//   some procedure. It's life begins when the procedure is requested. It calls
//   back when the procedure is done, and it is then the responsibility of the
//   caller to free the Synchro.
// Some Synchros call back asynchronously (from the procedure's thread), and
//   some must be polled.
//

namespace Sapphire {

    enum {
        SynchroFailure,
        SynchroSuccess,
    };

    struct _PSpriv;
    struct _CBSpriv;

    //The superclass that all Synchro objects derive from.
    class Synchro {
public:
        //Construction and destruction is performed by the caller.
        Synchro();
        virtual ~Synchro();

        // Readying is performed by the procedure.
        virtual void Ready(int) = 0;
private:
        void *nn;
    };

    class PollSynchro : public Sapphire::Synchro {
public:
        PollSynchro();
        virtual ~PollSynchro();

        virtual void Ready(int);

        // A function to call until the synchro has completed.
        virtual bool isDone(void);
private:
        _PSpriv *_p;

    };

    class CallbackSynchro : public Sapphire::Synchro {
public:
        CallbackSynchro();
        virtual ~CallbackSynchro();

        virtual void Ready(int);

        // A function that is called by the procedure when it is finished.
        virtual int callback(void *p);
private:
        _CBSpriv *_p;

    };


}
#endif
