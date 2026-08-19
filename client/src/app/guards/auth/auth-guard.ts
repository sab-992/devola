import { isPlatformBrowser } from '@angular/common';
import { inject, PLATFORM_ID } from '@angular/core';
import { CanActivateFn, Router } from '@angular/router';
import { UserService } from '@services/user/user';


export const authGuard: CanActivateFn = (_, __) => {
    if (isPlatformBrowser(inject(PLATFORM_ID)) && !inject(UserService).isAuthenticated())
        return inject(Router).createUrlTree(['/']);

    return true;
};
