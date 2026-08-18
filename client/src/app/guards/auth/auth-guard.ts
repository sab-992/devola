import { inject } from '@angular/core';
import { CanActivateFn, Router } from '@angular/router';
import { UserService } from '@services/user/user';


export const authGuard: CanActivateFn = (_, __) => {
    if (inject(UserService).isAuthenticated())
        return true;

    return inject(Router).createUrlTree(['/']);
};
